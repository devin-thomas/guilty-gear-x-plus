/* The renderer on SDL_GPU (dl_gpu.h). */
#include "dl_gpu.h"

#include <stdlib.h>
#include <string.h>

#include "shaders/dl_shaders.h"

#define CLUT_W 256
#define MAX_PIPELINES 32
/* frames a texture may go unused before its GPU copy is dropped */
#define TEX_AGE 120

typedef struct {
    uint64_t key;
    SDL_GPUGraphicsPipeline *p;
} Pipe;

typedef struct {
    int32_t mode, tcc, tfx, clut_row;
    int32_t wrap[2], size[2];
    float clut_inv[2];
    float fix;
    int32_t gs_blend;
    int32_t sel[4];
} DrawPS;

/* shaders/display.frag's uniforms: per circuit, its picture on the output,
   where it starts in its surface and the surface pixels per output pixel */
typedef struct {
    int32_t rect[2][4];
    int32_t src[2][4];
    float step[2][4];
    float bg[4];
    int32_t mode[4];
    int32_t fb_size[4];
} DisplayPS;

struct DlGpu {
    SDL_GPUDevice *dev;
    DlGpuOptions opt;
    SDL_GPUShaderFormat fmt;
    SDL_GPUShader *draw_vs, *draw_fs, *full_vs, *display_fs;
    Pipe pipes[MAX_PIPELINES];
    int npipes;
    SDL_GPUGraphicsPipeline *display;
    SDL_GPUTexture *surf[DL_MAX_SURFACES];   /* the frames' surfaces, by index */
    uint32_t nsurf;
    SDL_GPUTexture *output, *clut, *dummy;
    SDL_GPUTexture *dest;    /* a copy of a surface, for a gs_blend batch or a blit within one */
    uint32_t w, h;
    SDL_GPUSampler *point;
    SDL_GPUBuffer *vbuf;
    uint32_t vbuf_size;
    SDL_GPUTransferBuffer *up;
    uint32_t up_size;
    SDL_GPUTransferBuffer *down;
    uint32_t drawn;          /* the serial of the last frame drawn */
    /* the host's overlay (dl_gpu_set_overlay): a 640x448 picture over the output */
    uint8_t *ovl_rgba;
    int ovl_on, ovl_dirty;
    SDL_GPUTexture *ovl;
    SDL_GPUBuffer *ovl_vbuf;
    SDL_GPUTransferBuffer *ovl_up;
};

static SDL_GPUShader *shader(DlGpu *g, int frag, const unsigned char *spv, size_t nspv, const unsigned char *msl,
                             size_t nmsl, const unsigned char *dxil, size_t ndxil, int samplers, int uniforms)
{
    SDL_GPUShaderCreateInfo ci;

    memset(&ci, 0, sizeof(ci));
    ci.stage = frag ? SDL_GPU_SHADERSTAGE_FRAGMENT : SDL_GPU_SHADERSTAGE_VERTEX;
    ci.num_samplers = (Uint32)samplers;
    ci.num_uniform_buffers = (Uint32)uniforms;
    if (g->fmt & SDL_GPU_SHADERFORMAT_SPIRV) {
        ci.format = SDL_GPU_SHADERFORMAT_SPIRV, ci.code = spv, ci.code_size = nspv, ci.entrypoint = "main";
    } else if (g->fmt & SDL_GPU_SHADERFORMAT_MSL) {
        ci.format = SDL_GPU_SHADERFORMAT_MSL, ci.code = msl, ci.code_size = nmsl - 1, ci.entrypoint = "main0";
    } else {
        ci.format = SDL_GPU_SHADERFORMAT_DXIL, ci.code = dxil, ci.code_size = ndxil, ci.entrypoint = "main";
    }
    return SDL_CreateGPUShader(g->dev, &ci);
}

#define SHADER(g, frag, n, s, u) \
    shader(g, frag, dl_##n##_spirv, sizeof(dl_##n##_spirv), dl_##n##_msl, sizeof(dl_##n##_msl), dl_##n##_dxil, \
           sizeof(dl_##n##_dxil), s, u)

/* The GS's blend, (A - B) * C + D, as fixed-function blending: each of Cs
   and Cd ends up with a factor m * C + k (m -1, 0 or 1; k 0 or 1). */
static int blend_state(const DlBlend *b, SDL_GPUColorTargetBlendState *st)
{
    SDL_GPUBlendFactor cf = b->c == 0 ? SDL_GPU_BLENDFACTOR_SRC_ALPHA
                          : b->c == 1 ? SDL_GPU_BLENDFACTOR_DST_ALPHA
                                      : SDL_GPU_BLENDFACTOR_CONSTANT_COLOR;
    SDL_GPUBlendFactor ncf = b->c == 0 ? SDL_GPU_BLENDFACTOR_ONE_MINUS_SRC_ALPHA
                           : b->c == 1 ? SDL_GPU_BLENDFACTOR_ONE_MINUS_DST_ALPHA
                                       : SDL_GPU_BLENDFACTOR_ONE_MINUS_CONSTANT_COLOR;
    int m[2], k[2], i;
    SDL_GPUBlendFactor f[2];

    memset(st, 0, sizeof(*st));
    st->color_write_mask = SDL_GPU_COLORCOMPONENT_R | SDL_GPU_COLORCOMPONENT_G | SDL_GPU_COLORCOMPONENT_B |
                           SDL_GPU_COLORCOMPONENT_A;
    st->enable_color_write_mask = true;
    if (!b->abe) {
        return 0;
    }
    for (i = 0; i < 2; i++) {
        m[i] = (b->a == i) - (b->b == i);
        k[i] = b->d == i;
    }
    for (i = 0; i < 2; i++) {
        if (m[i] == 0) {
            f[i] = k[i] ? SDL_GPU_BLENDFACTOR_ONE : SDL_GPU_BLENDFACTOR_ZERO;
        } else if (m[i] == 1) {
            if (k[i]) {
                return -1;   /* 1 + C: not expressible */
            }
            f[i] = cf;
        } else {
            f[i] = k[i] ? ncf : cf;
        }
    }
    st->enable_blend = true;
    st->src_color_blendfactor = f[0];
    st->dst_color_blendfactor = f[1];
    st->color_blend_op = SDL_GPU_BLENDOP_ADD;
    if (m[0] < 0 && !k[0]) {
        if (m[1] < 0 && !k[1]) {
            return -1;
        }
        st->color_blend_op = SDL_GPU_BLENDOP_REVERSE_SUBTRACT;
    } else if (m[1] < 0 && !k[1]) {
        st->color_blend_op = SDL_GPU_BLENDOP_SUBTRACT;
    }
    /* the GS writes As to the framebuffer's alpha; nothing reads it back */
    st->src_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ONE;
    st->dst_alpha_blendfactor = SDL_GPU_BLENDFACTOR_ZERO;
    st->alpha_blend_op = SDL_GPU_BLENDOP_ADD;
    return 0;
}

static uint64_t blend_key(const DlBlend *b)
{
    return b->abe ? 1 | (uint64_t)b->a << 8 | (uint64_t)b->b << 16 | (uint64_t)b->c << 24 | (uint64_t)b->d << 32 : 0;
}

static SDL_GPUGraphicsPipeline *draw_pipeline(DlGpu *g, const DlBlend *b)
{
    uint64_t key = blend_key(b);
    SDL_GPUGraphicsPipelineCreateInfo ci;
    SDL_GPUVertexBufferDescription vb;
    SDL_GPUVertexAttribute va[3];
    SDL_GPUColorTargetDescription ct;
    int i;

    for (i = 0; i < g->npipes; i++) {
        if (g->pipes[i].key == key) {
            return g->pipes[i].p;
        }
    }
    if (g->npipes == MAX_PIPELINES) {
        return NULL;
    }
    memset(&ci, 0, sizeof(ci));
    memset(&vb, 0, sizeof(vb));
    memset(va, 0, sizeof(va));
    memset(&ct, 0, sizeof(ct));
    if (blend_state(b, &ct.blend_state) != 0) {
        return NULL;
    }
    vb.slot = 0;
    vb.pitch = sizeof(DlVertex);
    vb.input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX;
    va[0].location = 0, va[0].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, va[0].offset = 0;
    va[1].location = 1, va[1].format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2, va[1].offset = 8;
    va[2].location = 2, va[2].format = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM, va[2].offset = 16;
    ci.vertex_shader = g->draw_vs;
    ci.fragment_shader = g->draw_fs;
    ci.vertex_input_state.vertex_buffer_descriptions = &vb;
    ci.vertex_input_state.num_vertex_buffers = 1;
    ci.vertex_input_state.vertex_attributes = va;
    ci.vertex_input_state.num_vertex_attributes = 3;
    ci.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    ci.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    ci.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
    ct.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    ci.target_info.color_target_descriptions = &ct;
    ci.target_info.num_color_targets = 1;
    g->pipes[g->npipes].key = key;
    g->pipes[g->npipes].p = SDL_CreateGPUGraphicsPipeline(g->dev, &ci);
    if (!g->pipes[g->npipes].p) {
        return NULL;
    }
    return g->pipes[g->npipes++].p;
}

static SDL_GPUTexture *texture(DlGpu *g, uint32_t w, uint32_t h, SDL_GPUTextureFormat f, SDL_GPUTextureUsageFlags u)
{
    SDL_GPUTextureCreateInfo ci;

    memset(&ci, 0, sizeof(ci));
    ci.type = SDL_GPU_TEXTURETYPE_2D;
    ci.format = f;
    ci.usage = u;
    ci.width = w;
    ci.height = h;
    ci.layer_count_or_depth = 1;
    ci.num_levels = 1;
    return SDL_CreateGPUTexture(g->dev, &ci);
}

DlGpu *dl_gpu_new(SDL_GPUDevice *dev, const DlGpuOptions *opt)
{
    DlGpu *g = calloc(1, sizeof(DlGpu));
    SDL_GPUSamplerCreateInfo sc;
    SDL_GPUGraphicsPipelineCreateInfo ci;
    SDL_GPUColorTargetDescription ct;
    SDL_GPUTransferBufferCreateInfo tb;

    if (!g) {
        return NULL;
    }
    g->dev = dev;
    g->opt = *opt;
    if (g->opt.scale < 1) {
        g->opt.scale = 1;
    }
    if (g->opt.scale > 8) {
        g->opt.scale = 8;
    }
    g->w = DL_FB_W * (uint32_t)g->opt.scale;
    g->h = DL_FB_H * (uint32_t)g->opt.scale;
    g->fmt = SDL_GetGPUShaderFormats(dev);
    g->draw_vs = SHADER(g, 0, draw_vert, 0, 1);
    g->draw_fs = SHADER(g, 1, draw_frag, 3, 1);
    g->full_vs = SHADER(g, 0, full_vert, 0, 0);
    g->display_fs = SHADER(g, 1, display_frag, 2, 1);
    if (!g->draw_vs || !g->draw_fs || !g->full_vs || !g->display_fs) {
        SDL_Log("ggx: shader: %s", SDL_GetError());
        dl_gpu_free(g);
        return NULL;
    }
    memset(&ci, 0, sizeof(ci));
    memset(&ct, 0, sizeof(ct));
    ct.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM;
    ci.vertex_shader = g->full_vs;
    ci.fragment_shader = g->display_fs;
    ci.primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST;
    ci.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    ci.rasterizer_state.cull_mode = SDL_GPU_CULLMODE_NONE;
    ci.target_info.color_target_descriptions = &ct;
    ci.target_info.num_color_targets = 1;
    g->display = SDL_CreateGPUGraphicsPipeline(dev, &ci);
    g->output = texture(g, g->w, g->h, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
                        SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER);
    g->dest = texture(g, g->w, g->h, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, SDL_GPU_TEXTUREUSAGE_SAMPLER);
    g->clut = texture(g, CLUT_W, DL_CLUT_ROWS, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, SDL_GPU_TEXTUREUSAGE_SAMPLER);
    g->dummy = texture(g, 1, 1, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, SDL_GPU_TEXTUREUSAGE_SAMPLER);
    memset(&sc, 0, sizeof(sc));
    sc.min_filter = SDL_GPU_FILTER_NEAREST;
    sc.mag_filter = SDL_GPU_FILTER_NEAREST;
    sc.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST;
    sc.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    sc.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    sc.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE;
    g->point = SDL_CreateGPUSampler(dev, &sc);
    memset(&tb, 0, sizeof(tb));
    tb.usage = SDL_GPU_TRANSFERBUFFERUSAGE_DOWNLOAD;
    tb.size = g->w * g->h * 4;
    g->down = SDL_CreateGPUTransferBuffer(dev, &tb);
    if (!g->display || !g->output || !g->dest || !g->clut || !g->dummy || !g->point || !g->down) {
        SDL_Log("ggx: GPU resources: %s", SDL_GetError());
        dl_gpu_free(g);
        return NULL;
    }
    return g;
}

void dl_gpu_free(DlGpu *g)
{
    int i;

    if (!g) {
        return;
    }
    for (i = 0; i < g->npipes; i++) {
        SDL_ReleaseGPUGraphicsPipeline(g->dev, g->pipes[i].p);
    }
    if (g->display) {
        SDL_ReleaseGPUGraphicsPipeline(g->dev, g->display);
    }
    if (g->draw_vs) {
        SDL_ReleaseGPUShader(g->dev, g->draw_vs);
    }
    if (g->draw_fs) {
        SDL_ReleaseGPUShader(g->dev, g->draw_fs);
    }
    if (g->full_vs) {
        SDL_ReleaseGPUShader(g->dev, g->full_vs);
    }
    if (g->display_fs) {
        SDL_ReleaseGPUShader(g->dev, g->display_fs);
    }
    for (i = 0; i < (int)g->nsurf; i++) {
        SDL_ReleaseGPUTexture(g->dev, g->surf[i]);
    }
    if (g->output) {
        SDL_ReleaseGPUTexture(g->dev, g->output);
    }
    if (g->dest) {
        SDL_ReleaseGPUTexture(g->dev, g->dest);
    }
    if (g->clut) {
        SDL_ReleaseGPUTexture(g->dev, g->clut);
    }
    if (g->dummy) {
        SDL_ReleaseGPUTexture(g->dev, g->dummy);
    }
    if (g->point) {
        SDL_ReleaseGPUSampler(g->dev, g->point);
    }
    if (g->vbuf) {
        SDL_ReleaseGPUBuffer(g->dev, g->vbuf);
    }
    if (g->up) {
        SDL_ReleaseGPUTransferBuffer(g->dev, g->up);
    }
    if (g->down) {
        SDL_ReleaseGPUTransferBuffer(g->dev, g->down);
    }
    if (g->ovl) {
        SDL_ReleaseGPUTexture(g->dev, g->ovl);
    }
    if (g->ovl_vbuf) {
        SDL_ReleaseGPUBuffer(g->dev, g->ovl_vbuf);
    }
    if (g->ovl_up) {
        SDL_ReleaseGPUTransferBuffer(g->dev, g->ovl_up);
    }
    free(g->ovl_rgba);
    free(g);
}

#define OVL_BYTES (DL_FB_W * DL_FB_H * 4)

int dl_gpu_set_overlay(DlGpu *g, const uint8_t *rgba)
{
    if (!rgba) {
        g->ovl_on = 0;
        return 0;
    }
    if (!g->ovl_rgba && !(g->ovl_rgba = malloc(OVL_BYTES))) {
        return -1;
    }
    memcpy(g->ovl_rgba, rgba, OVL_BYTES);
    g->ovl_on = g->ovl_dirty = 1;
    return 0;
}

/* The overlay over the output: one quad, blended as a GS sprite with its
   alpha (0x80 opaque), through the draw pipeline. */
static int overlay(DlGpu *g, SDL_GPUCommandBuffer *cmd)
{
    static const DlBlend alpha = { 1, 0, 1, 0, 1, 0 };   /* (Cs - Cd) * As + Cd */
    SDL_GPUGraphicsPipeline *p = draw_pipeline(g, &alpha);
    SDL_GPUColorTargetInfo ti;
    SDL_GPURenderPass *rp;
    SDL_GPUTextureSamplerBinding bind[3];
    SDL_GPUBufferBinding vb;
    float inv[4] = { 2.0f / DL_FB_W, 2.0f / DL_FB_H, 0, 0 };
    SDL_Rect sr = { 0, 0, (int)g->w, (int)g->h };
    DrawPS u;

    if (!p) {
        return -1;
    }
    if (!g->ovl) {
        SDL_GPUBufferCreateInfo bc;
        SDL_GPUTransferBufferCreateInfo tb;

        g->ovl = texture(g, DL_FB_W, DL_FB_H, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM, SDL_GPU_TEXTUREUSAGE_SAMPLER);
        memset(&bc, 0, sizeof(bc));
        bc.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
        bc.size = 6 * sizeof(DlVertex);
        g->ovl_vbuf = SDL_CreateGPUBuffer(g->dev, &bc);
        memset(&tb, 0, sizeof(tb));
        tb.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        tb.size = OVL_BYTES + 256;
        g->ovl_up = SDL_CreateGPUTransferBuffer(g->dev, &tb);
        if (!g->ovl || !g->ovl_vbuf || !g->ovl_up) {
            return -1;
        }
        g->ovl_dirty = 1;
    }
    if (g->ovl_dirty) {
        static const float q[6][2] = { { 0, 0 }, { DL_FB_W, 0 }, { 0, DL_FB_H },
                                       { DL_FB_W, 0 }, { DL_FB_W, DL_FB_H }, { 0, DL_FB_H } };
        DlVertex v[6];
        SDL_GPUCopyPass *cp;
        uint8_t *map = SDL_MapGPUTransferBuffer(g->dev, g->ovl_up, true);
        SDL_GPUTextureTransferInfo src = { g->ovl_up, 0, DL_FB_W, DL_FB_H };
        SDL_GPUTextureRegion dst;
        SDL_GPUTransferBufferLocation vsrc = { g->ovl_up, OVL_BYTES };
        SDL_GPUBufferRegion vdst = { g->ovl_vbuf, 0, sizeof(v) };
        int i;

        if (!map) {
            return -1;
        }
        for (i = 0; i < 6; i++) {
            v[i].x = q[i][0], v[i].y = q[i][1], v[i].u = q[i][0], v[i].v = q[i][1];
            v[i].r = v[i].g = v[i].b = v[i].a = 0x80;
        }
        memcpy(map, g->ovl_rgba, OVL_BYTES);
        memcpy(map + OVL_BYTES, v, sizeof(v));
        SDL_UnmapGPUTransferBuffer(g->dev, g->ovl_up);
        memset(&dst, 0, sizeof(dst));
        dst.texture = g->ovl, dst.w = DL_FB_W, dst.h = DL_FB_H, dst.d = 1;
        cp = SDL_BeginGPUCopyPass(cmd);
        SDL_UploadToGPUTexture(cp, &src, &dst, false);
        SDL_UploadToGPUBuffer(cp, &vsrc, &vdst, false);
        SDL_EndGPUCopyPass(cp);
        g->ovl_dirty = 0;
    }
    memset(&ti, 0, sizeof(ti));
    ti.texture = g->output;
    ti.load_op = SDL_GPU_LOADOP_LOAD;
    ti.store_op = SDL_GPU_STOREOP_STORE;
    rp = SDL_BeginGPURenderPass(cmd, &ti, 1, NULL);
    SDL_BindGPUGraphicsPipeline(rp, p);
    vb.buffer = g->ovl_vbuf;
    vb.offset = 0;
    SDL_BindGPUVertexBuffers(rp, 0, &vb, 1);
    SDL_SetGPUScissor(rp, &sr);
    SDL_PushGPUVertexUniformData(cmd, 0, inv, sizeof(inv));
    memset(&u, 0, sizeof(u));
    u.mode = 1;
    u.tcc = 1;
    u.wrap[0] = 1023, u.wrap[1] = 511;
    u.size[0] = DL_FB_W, u.size[1] = DL_FB_H;
    u.clut_inv[0] = 1.0f / CLUT_W;
    u.clut_inv[1] = 1.0f / DL_CLUT_ROWS;
    u.sel[0] = 0, u.sel[1] = 1, u.sel[2] = 0, u.sel[3] = 1;
    bind[0].texture = g->ovl, bind[0].sampler = g->point;
    bind[1].texture = g->clut, bind[1].sampler = g->point;
    bind[2].texture = g->dummy, bind[2].sampler = g->point;
    SDL_BindGPUFragmentSamplers(rp, 0, bind, 3);
    SDL_PushGPUFragmentUniformData(cmd, 0, &u, sizeof(u));
    SDL_DrawGPUPrimitives(rp, 6, 1, 0, 0);
    SDL_EndGPURenderPass(rp);
    return 0;
}

SDL_GPUTexture *dl_gpu_output(const DlGpu *g, uint32_t *w, uint32_t *h)
{
    *w = g->w;
    *h = g->h;
    return g->output;
}

static uint32_t tex_bytes(const DlTex *t)
{
    return (uint32_t)t->w * t->h * (t->kind == DL_TEX_RGBA ? 4u : 1u);
}

/* Everything this frame sends to the GPU, in one transfer buffer: the
   vertices, the CLUT rows and the textures not uploaded yet. */
static int upload(DlGpu *g, SDL_GPUCommandBuffer *cmd, const DlFrame *f, DlTexCache *tc)
{
    uint32_t need = f->nv * (uint32_t)sizeof(DlVertex) + f->nclut * CLUT_W * 4, off, i;
    SDL_GPUCopyPass *cp;
    uint8_t *map;

    for (i = 0; i < f->nb; i++) {
        const DlBatch *b = &f->b[i];

        if (b->tex >= 0 && tc->e[b->tex].pixels && !tc->e[b->tex].gpu) {
            DlTex *t = &tc->e[b->tex];

            t->gpu = texture(g, t->w, t->h,
                             t->kind == DL_TEX_RGBA ? SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM
                                                    : SDL_GPU_TEXTUREFORMAT_R8_UNORM,
                             SDL_GPU_TEXTUREUSAGE_SAMPLER);
            if (!t->gpu) {
                return -1;
            }
            need += (tex_bytes(t) + 255) & ~255u;
        }
    }
    if (need == 0) {
        return 0;
    }
    if (need > g->up_size) {
        SDL_GPUTransferBufferCreateInfo tb;

        if (g->up) {
            SDL_ReleaseGPUTransferBuffer(g->dev, g->up);
        }
        memset(&tb, 0, sizeof(tb));
        tb.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
        g->up_size = need + need / 2;
        tb.size = g->up_size;
        g->up = SDL_CreateGPUTransferBuffer(g->dev, &tb);
        if (!g->up) {
            return -1;
        }
    }
    if (f->nv * sizeof(DlVertex) > g->vbuf_size) {
        SDL_GPUBufferCreateInfo bc;

        if (g->vbuf) {
            SDL_ReleaseGPUBuffer(g->dev, g->vbuf);
        }
        memset(&bc, 0, sizeof(bc));
        bc.usage = SDL_GPU_BUFFERUSAGE_VERTEX;
        g->vbuf_size = (uint32_t)(f->nv * sizeof(DlVertex) * 2);
        bc.size = g->vbuf_size;
        g->vbuf = SDL_CreateGPUBuffer(g->dev, &bc);
        if (!g->vbuf) {
            return -1;
        }
    }
    map = SDL_MapGPUTransferBuffer(g->dev, g->up, true);
    if (!map) {
        return -1;
    }
    cp = SDL_BeginGPUCopyPass(cmd);
    off = 0;
    if (f->nv) {
        SDL_GPUTransferBufferLocation src = { g->up, 0 };
        SDL_GPUBufferRegion dst = { g->vbuf, 0, f->nv * (uint32_t)sizeof(DlVertex) };

        memcpy(map, f->v, dst.size);
        off = (dst.size + 255) & ~255u;
        SDL_UploadToGPUBuffer(cp, &src, &dst, true);
    }
    if (f->nclut) {
        SDL_GPUTextureTransferInfo src = { g->up, off, CLUT_W, f->nclut };
        SDL_GPUTextureRegion dst;

        memset(&dst, 0, sizeof(dst));
        dst.texture = g->clut, dst.w = CLUT_W, dst.h = f->nclut, dst.d = 1;
        memcpy(map + off, f->clut, (size_t)f->nclut * CLUT_W * 4);
        off += (f->nclut * CLUT_W * 4 + 255) & ~255u;
        SDL_UploadToGPUTexture(cp, &src, &dst, false);
    }
    for (i = 0; i < f->nb; i++) {
        const DlBatch *b = &f->b[i];
        DlTex *t = b->tex >= 0 ? &tc->e[b->tex] : NULL;

        if (t && t->pixels && t->gpu) {
            SDL_GPUTextureTransferInfo src = { g->up, off, t->w, t->h };
            SDL_GPUTextureRegion dst;

            memset(&dst, 0, sizeof(dst));
            dst.texture = t->gpu, dst.w = t->w, dst.h = t->h, dst.d = 1;
            memcpy(map + off, t->pixels, tex_bytes(t));
            off += (tex_bytes(t) + 255) & ~255u;
            SDL_UploadToGPUTexture(cp, &src, &dst, false);
            free(t->pixels);
            t->pixels = NULL;
        }
    }
    SDL_UnmapGPUTransferBuffer(g->dev, g->up);
    SDL_EndGPUCopyPass(cp);
    return 0;
}

/* The frame's surfaces not created yet, each filled once with what local
   memory held there (DlSurface init), scaled up with nearest sampling. From
   then on a surface keeps what is drawn into it, as local memory does. */
static int new_surfaces(DlGpu *g, SDL_GPUCommandBuffer *cmd, const DlFrame *f)
{
    while (g->nsurf < f->nsurf) {
        const uint8_t *init = f->surf[g->nsurf].init;
        SDL_GPUTexture *t = texture(g, g->w, g->h, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
                                    SDL_GPU_TEXTUREUSAGE_COLOR_TARGET | SDL_GPU_TEXTUREUSAGE_SAMPLER);
        SDL_GPUTexture *one = init ? texture(g, DL_FB_W, DL_FB_H, SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
                                             SDL_GPU_TEXTUREUSAGE_SAMPLER)
                                   : NULL;
        SDL_GPUTransferBufferCreateInfo tb;
        SDL_GPUTransferBuffer *buf = NULL;

        if (!t || (init && !one)) {
            return -1;
        }
        g->surf[g->nsurf++] = t;
        if (init) {
            SDL_GPUTextureTransferInfo src;
            SDL_GPUTextureRegion dst;
            SDL_GPUBlitInfo bi;
            SDL_GPUCopyPass *cp;
            void *map;

            memset(&tb, 0, sizeof(tb));
            tb.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD;
            tb.size = DL_FB_W * DL_FB_H * 4;
            buf = SDL_CreateGPUTransferBuffer(g->dev, &tb);
            map = buf ? SDL_MapGPUTransferBuffer(g->dev, buf, false) : NULL;
            if (!map) {
                return -1;
            }
            memcpy(map, init, tb.size);
            SDL_UnmapGPUTransferBuffer(g->dev, buf);
            memset(&src, 0, sizeof(src));
            src.transfer_buffer = buf;
            memset(&dst, 0, sizeof(dst));
            dst.texture = one, dst.w = DL_FB_W, dst.h = DL_FB_H, dst.d = 1;
            cp = SDL_BeginGPUCopyPass(cmd);
            SDL_UploadToGPUTexture(cp, &src, &dst, false);
            SDL_EndGPUCopyPass(cp);
            memset(&bi, 0, sizeof(bi));
            bi.source.texture = one, bi.source.w = DL_FB_W, bi.source.h = DL_FB_H;
            bi.destination.texture = t, bi.destination.w = g->w, bi.destination.h = g->h;
            bi.load_op = SDL_GPU_LOADOP_DONT_CARE;
            bi.filter = SDL_GPU_FILTER_NEAREST;
            SDL_BlitGPUTexture(cmd, &bi);
            /* released once the command buffer is done with them */
            SDL_ReleaseGPUTransferBuffer(g->dev, buf);
            SDL_ReleaseGPUTexture(g->dev, one);
        } else {
            SDL_GPUColorTargetInfo ti;

            memset(&ti, 0, sizeof(ti));
            ti.texture = t;
            ti.load_op = SDL_GPU_LOADOP_CLEAR;
            ti.store_op = SDL_GPU_STOREOP_STORE;
            SDL_EndGPURenderPass(SDL_BeginGPURenderPass(cmd, &ti, 1, NULL));
        }
    }
    return 0;
}

static void copy_region(SDL_GPUCommandBuffer *cmd, SDL_GPUTexture *from, uint32_t sx, uint32_t sy,
                        SDL_GPUTexture *to, uint32_t dx, uint32_t dy, uint32_t w, uint32_t h)
{
    SDL_GPUCopyPass *cp = SDL_BeginGPUCopyPass(cmd);
    SDL_GPUTextureLocation a, b;

    memset(&a, 0, sizeof(a));
    memset(&b, 0, sizeof(b));
    a.texture = from, a.x = sx, a.y = sy;
    b.texture = to, b.x = dx, b.y = dy;
    SDL_CopyGPUTextureToTexture(cp, &a, &b, w, h, 1, false);
    SDL_EndGPUCopyPass(cp);
}

/* A local-to-local transfer between surfaces, at the internal scale. One
   within a single surface goes through the scratch copy: a GPU copy may not
   overlap itself. */
static void blit(DlGpu *g, SDL_GPUCommandBuffer *cmd, const DlBlit *k)
{
    uint32_t s = (uint32_t)g->opt.scale;

    if (k->src == k->dst) {
        copy_region(cmd, g->surf[k->src], k->sx * s, k->sy * s, g->dest, k->sx * s, k->sy * s, k->w * s, k->h * s);
        copy_region(cmd, g->dest, k->sx * s, k->sy * s, g->surf[k->dst], k->dx * s, k->dy * s, k->w * s, k->h * s);
    } else {
        copy_region(cmd, g->surf[k->src], k->sx * s, k->sy * s, g->surf[k->dst], k->dx * s, k->dy * s, k->w * s,
                    k->h * s);
    }
}

/* The display stage's uniforms from the frame's DlDisplay, at the internal
   scale. Without the merge (--merge off), circuit 2 is moved onto circuit
   1's lines when they show the same buffer less than four lines apart, as
   PCSX2's anti-blur does, so each line blends with itself: the frame as
   drawn. */
static void display_uniforms(const DlGpu *g, const DlDisplay *d, DisplayPS *u)
{
    int s = g->opt.scale, i;

    memset(u, 0, sizeof(*u));
    for (i = 0; i < 2; i++) {
        const DlCircuit *c = &d->c[i];
        int y = c->y;

        if (!c->on) {
            continue;
        }
        if (i == 1 && !g->opt.display_merge && d->c[0].on && d->c[0].surface == c->surface &&
            d->c[0].dbx == c->dbx && d->c[0].dby == c->dby && abs(y - d->c[0].y) < 4) {
            y = d->c[0].y;
        }
        u->rect[i][0] = c->x * s;
        u->rect[i][1] = y * s;
        u->rect[i][2] = c->w * s;
        u->rect[i][3] = c->h * s;
        u->src[i][0] = c->dbx * s;
        u->src[i][1] = c->dby * s;
        u->step[i][0] = 4.0f / (float)(c->magh + 1);
        u->step[i][1] = 1.0f / (float)((c->magv + 1) * (c->half ? 2 : 1));
    }
    u->bg[0] = (d->bgcolor & 0xFF) / 255.0f;
    u->bg[1] = ((d->bgcolor >> 8) & 0xFF) / 255.0f;
    u->bg[2] = ((d->bgcolor >> 16) & 0xFF) / 255.0f;
    u->bg[3] = 1.0f;
    u->mode[0] = d->mmod;
    u->mode[1] = d->slbg;
    u->mode[2] = d->alp;
    u->fb_size[0] = (int32_t)g->w;
    u->fb_size[1] = (int32_t)g->h;
}

int dl_gpu_render(DlGpu *g, SDL_GPUCommandBuffer *cmd, const DlFrame *f, DlTexCache *tc)
{
    SDL_GPUColorTargetInfo ti;
    SDL_GPURenderPass *rp = NULL;
    SDL_GPUGraphicsPipeline *bound = NULL;
    SDL_GPUTextureSamplerBinding bind[3];
    SDL_GPUBufferBinding vb;
    float inv[4] = { 2.0f / DL_FB_W, 2.0f / DL_FB_H, 0, 0 };
    DisplayPS dp;
    uint32_t i, s = (uint32_t)g->opt.scale;
    int rc = 0, cur = -1;

    if (upload(g, cmd, f, tc) != 0 || new_surfaces(g, cmd, f) != 0) {
        SDL_Log("ggx: GPU upload: %s", SDL_GetError());
        return -1;
    }
    memset(&ti, 0, sizeof(ti));
    ti.load_op = SDL_GPU_LOADOP_LOAD;
    ti.store_op = SDL_GPU_STOREOP_STORE;
    vb.buffer = g->vbuf;
    vb.offset = 0;
    SDL_PushGPUVertexUniformData(cmd, 0, inv, sizeof(inv));
    for (i = 0; i < f->nb; i++) {
        const DlBatch *b = &f->b[i];
        DlBlend none = { 0 };
        SDL_GPUGraphicsPipeline *p;
        SDL_Rect sr;
        DrawPS u;

        if (b->blit >= 0) {
            /* a transfer between framebuffers, in its place in the stream */
            if (rp) {
                SDL_EndGPURenderPass(rp);
                rp = NULL;
            }
            blit(g, cmd, &f->blit[b->blit]);
            continue;
        }
        p = draw_pipeline(g, b->gs_blend ? &none : &b->blend);
        if (!p || b->surface >= g->nsurf) {
            rc = -1;
            continue;
        }
        if (b->gs_blend) {
            /* the shader blends with what the framebuffer holds now: copy
               it, and carry on in a new pass */
            if (rp) {
                SDL_EndGPURenderPass(rp);
                rp = NULL;
            }
            copy_region(cmd, g->surf[b->surface], 0, 0, g->dest, 0, 0, g->w, g->h);
        }
        if (!rp || cur != b->surface) {
            if (rp) {
                SDL_EndGPURenderPass(rp);
            }
            ti.texture = g->surf[b->surface];
            rp = SDL_BeginGPURenderPass(cmd, &ti, 1, NULL);
            SDL_BindGPUVertexBuffers(rp, 0, &vb, 1);
            cur = b->surface;
            bound = NULL;
        }
        if (p != bound) {
            SDL_BindGPUGraphicsPipeline(rp, p);
            bound = p;
        }
        if (b->blend.abe && b->blend.c == 2) {
            SDL_FColor k = { b->blend.fix / 128.0f, b->blend.fix / 128.0f, b->blend.fix / 128.0f, 1.0f };

            SDL_SetGPUBlendConstants(rp, k);
        }
        sr.x = b->scissor[0] * (int)s;
        sr.y = b->scissor[1] * (int)s;
        sr.w = (b->scissor[2] - b->scissor[0] + 1) * (int)s;
        sr.h = (b->scissor[3] - b->scissor[1] + 1) * (int)s;
        if (sr.x + sr.w > (int)g->w) {
            sr.w = (int)g->w - sr.x;
        }
        if (sr.y + sr.h > (int)g->h) {
            sr.h = (int)g->h - sr.y;
        }
        if (sr.w <= 0 || sr.h <= 0) {
            continue;
        }
        SDL_SetGPUScissor(rp, &sr);
        memset(&u, 0, sizeof(u));
        u.mode = b->tex < 0 ? 0 : b->tex_kind == DL_TEX_INDEX ? 2 : 1;
        u.tcc = b->tcc;
        u.tfx = b->tfx;
        u.clut_row = b->clut;
        u.wrap[0] = (1 << b->tw) - 1;
        u.wrap[1] = (1 << b->th) - 1;
        u.size[0] = b->tex_w ? b->tex_w : 1;
        u.size[1] = b->tex_h ? b->tex_h : 1;
        u.clut_inv[0] = 1.0f / CLUT_W;
        u.clut_inv[1] = 1.0f / DL_CLUT_ROWS;
        u.fix = b->blend.fix;
        u.gs_blend = b->gs_blend;
        u.sel[0] = b->blend.a, u.sel[1] = b->blend.b, u.sel[2] = b->blend.c, u.sel[3] = b->blend.d;
        bind[0].texture = b->tex >= 0 && tc->e[b->tex].gpu ? (SDL_GPUTexture *)tc->e[b->tex].gpu : g->dummy;
        bind[0].sampler = g->point;
        bind[1].texture = g->clut;
        bind[1].sampler = g->point;
        bind[2].texture = b->gs_blend ? g->dest : g->dummy;
        bind[2].sampler = g->point;
        SDL_BindGPUFragmentSamplers(rp, 0, bind, 3);
        SDL_PushGPUFragmentUniformData(cmd, 0, &u, sizeof(u));
        SDL_DrawGPUPrimitives(rp, b->count, 1, b->first, 0);
    }
    if (rp) {
        SDL_EndGPURenderPass(rp);
    }

    memset(&ti, 0, sizeof(ti));
    ti.texture = g->output;
    ti.load_op = SDL_GPU_LOADOP_DONT_CARE;
    ti.store_op = SDL_GPU_STOREOP_STORE;
    rp = SDL_BeginGPURenderPass(cmd, &ti, 1, NULL);
    SDL_BindGPUGraphicsPipeline(rp, g->display);
    for (i = 0; i < 2; i++) {
        const DlCircuit *c = &f->display.c[i];

        bind[i].texture = c->on && c->surface < g->nsurf ? g->surf[c->surface] : g->dummy;
        bind[i].sampler = g->point;
    }
    SDL_BindGPUFragmentSamplers(rp, 0, bind, 2);
    display_uniforms(g, &f->display, &dp);
    SDL_PushGPUFragmentUniformData(cmd, 0, &dp, sizeof(dp));
    SDL_DrawGPUPrimitives(rp, 3, 1, 0, 0);
    SDL_EndGPURenderPass(rp);
    if (g->ovl_on && overlay(g, cmd) != 0) {
        SDL_Log("ggx: overlay: %s", SDL_GetError());
    }

    g->drawn = f->serial;
    dl_tex_evict(tc, f->serial, TEX_AGE);
    for (i = 0; i < tc->ndead; i++) {
        SDL_ReleaseGPUTexture(g->dev, tc->dead[i]);
    }
    tc->ndead = 0;
    return rc;
}

static void copy_out(DlGpu *g, SDL_GPUCommandBuffer *cmd, SDL_GPUTexture *tex, SDL_GPUTransferBuffer *buf)
{
    SDL_GPUCopyPass *cp;
    SDL_GPUTextureRegion src;
    SDL_GPUTextureTransferInfo dst;

    memset(&src, 0, sizeof(src));
    src.texture = tex, src.w = g->w, src.h = g->h, src.d = 1;
    memset(&dst, 0, sizeof(dst));
    dst.transfer_buffer = buf, dst.pixels_per_row = g->w, dst.rows_per_layer = g->h;
    cp = SDL_BeginGPUCopyPass(cmd);
    SDL_DownloadFromGPUTexture(cp, &src, &dst);
    SDL_EndGPUCopyPass(cp);
}

static int map_out(DlGpu *g, SDL_GPUTransferBuffer *buf, uint8_t *rgba)
{
    const uint8_t *map = SDL_MapGPUTransferBuffer(g->dev, buf, false);

    if (!map) {
        return -1;
    }
    memcpy(rgba, map, (size_t)g->w * g->h * 4);
    SDL_UnmapGPUTransferBuffer(g->dev, buf);
    return 0;
}

/* Submit cmd after copying tex into the download buffer, wait for it, and
   copy the pixels out. */
static int download(DlGpu *g, SDL_GPUCommandBuffer *cmd, SDL_GPUTexture *tex, uint8_t *rgba)
{
    SDL_GPUFence *fence;

    copy_out(g, cmd, tex, g->down);
    fence = SDL_SubmitGPUCommandBufferAndAcquireFence(cmd);
    if (!fence) {
        return -1;
    }
    SDL_WaitForGPUFences(g->dev, true, &fence, 1);
    SDL_ReleaseGPUFence(g->dev, fence);
    return map_out(g, g->down, rgba);
}

int dl_gpu_render_submit(DlGpu *g, const DlFrame *f, DlTexCache *tc)
{
    SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(g->dev);
    int rc;

    if (!cmd) {
        return -1;
    }
    rc = dl_gpu_render(g, cmd, f, tc);
    return SDL_SubmitGPUCommandBuffer(cmd) ? rc : -1;
}

int dl_gpu_render_read(DlGpu *g, const DlFrame *f, DlTexCache *tc, uint8_t *rgba)
{
    SDL_GPUCommandBuffer *cmd = SDL_AcquireGPUCommandBuffer(g->dev);
    int rc;

    if (!cmd) {
        return -1;
    }
    rc = dl_gpu_render(g, cmd, f, tc);
    return download(g, cmd, g->output, rgba) != 0 ? -1 : rc;
}

