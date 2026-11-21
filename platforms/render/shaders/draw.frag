#version 450
// The Draw Layer's primitives (PORT-008): the GS's texture function, in
// the GS's own integer arithmetic. Blending is the pipeline's (dl_gpu.c),
// except for a primitive whose As can exceed 1.0, which is blended here.

layout(location = 0) in vec4 v_col;
layout(location = 1) in vec2 v_uv;

layout(location = 0) out vec4 o_col;

layout(set = 2, binding = 0) uniform sampler2D tex;    // RGBA texels, or indices in .r
layout(set = 2, binding = 1) uniform sampler2D clut;   // the frame's CLUT atlas, one row a CLUT
layout(set = 2, binding = 2) uniform sampler2D dest;   // a copy of the framebuffer, for gs_blend

layout(set = 3, binding = 0) uniform DrawPS {
    int mode;           // 0 untextured, 1 RGBA texels, 2 indices into a CLUT row
    int tcc;            // TEX0 TCC: the texture's alpha is used
    int tfx;            // TEX0 TFX: 0 modulate, 1 decal, 2 highlight, 3 highlight2
    int clut_row;
    ivec2 wrap;         // (1 << TW) - 1, (1 << TH) - 1: CLAMP_1 is REPEAT
    ivec2 size;         // the texels stored
    vec2 clut_inv;      // 1 / the CLUT atlas size
    float fix;          // ALPHA FIX
    int gs_blend;       // blend here: (A - B) * C >> 7 + D, clamped (COLCLAMP)
    ivec4 sel;          // ALPHA A, B, C, D
};

// A blend input: 0 Cs, 1 Cd, 2 zero.
vec3 blend_in(int s, vec3 cs, vec3 cd)
{
    return s == 0 ? cs : s == 1 ? cd : vec3(0.0);
}

// A texel as 0-255 values, point sampled at its centre so the GPU never
// mixes neighbours.
vec4 fetch_tex(ivec2 p)
{
    return floor(textureLod(tex, (vec2(p) + 0.5) / vec2(size), 0.0) * 255.0 + 0.5);
}

void main()
{
    vec4 cv = floor(v_col * 255.0 + 0.5);
    vec4 c = cv;

    if (mode != 0) {
        // the GS's nearest texel, wrapped, kept inside what was stored (the
        // game never samples past its upload)
        ivec2 p = min(ivec2(floor(v_uv)) & wrap, size - 1);
        vec4 ct;

        if (mode == 2) {
            float k = fetch_tex(p).r;
            ct = floor(textureLod(clut, (vec2(k, float(clut_row)) + 0.5) * clut_inv, 0.0) * 255.0 + 0.5);
        } else {
            ct = fetch_tex(p);
        }
        if (tfx == 1) {
            c.rgb = ct.rgb;
            c.a = tcc != 0 ? ct.a : cv.a;
        } else if (tfx == 0) {
            c.rgb = min(floor(ct.rgb * cv.rgb / 128.0), 255.0);
            c.a = tcc != 0 ? min(floor(ct.a * cv.a / 128.0), 255.0) : cv.a;
        } else {
            c.rgb = min(floor(ct.rgb * cv.rgb / 128.0) + cv.a, vec3(255.0));
            c.a = tcc != 0 ? (tfx == 2 ? min(ct.a + cv.a, 255.0) : ct.a) : cv.a;
        }
    }
    if (gs_blend != 0) {
        // the GS's blend with As as it is, up to 0xFF / 0x80
        vec4 d = floor(texelFetch(dest, ivec2(gl_FragCoord.xy), 0) * 255.0 + 0.5);
        float k = sel.z == 0 ? c.a : sel.z == 1 ? min(floor(d.a * 128.0 / 255.0 + 0.5), 128.0) : fix;

        c.rgb = clamp(floor((blend_in(sel.x, c.rgb, d.rgb) - blend_in(sel.y, c.rgb, d.rgb)) * k / 128.0) +
                      blend_in(sel.w, c.rgb, d.rgb), 0.0, 255.0);
    }
    // the colour as stored; the alpha as the blend factor it is to the GS
    // (0x80 = 1.0; the colour target clamps anything above)
    o_col = vec4(c.rgb / 255.0, c.a / 128.0);
}
