/* The renderer on SDL_GPU: Vulkan, Metal or D3D12 (PORT-008, PORT-009).
 *
 * It draws a DlFrame into its framebuffers (surfaces) at 640x448 times an
 * integer scale, with the GS's blend equations as fixed-function blending
 * and its texture function in the fragment shader (shaders/draw.frag), does
 * the frame's blits between them where the stream had them, then runs the
 * display circuits (shaders/display.frag) into the output image a host
 * presents or reads back. It never touches the Core.
 */
#ifndef GGX_DL_GPU_H
#define GGX_DL_GPU_H

#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_log.h>

#include "dl_frame.h"
#include "dl_tex.h"

typedef struct DlGpu DlGpu;

typedef struct {
    int scale;            /* internal resolution: 640x448 times this (1-8) */
    int display_merge;    /* merge the display circuits as the game sets them; 0
                             shows the frame as drawn (PCSX2's anti-blur) */
} DlGpuOptions;

DlGpu *dl_gpu_new(SDL_GPUDevice *dev, const DlGpuOptions *opt);
void dl_gpu_free(DlGpu *g);

/* Draw a frame and run the display stage, recorded into cmd. Uploads the
   textures it needs from the cache (their CPU copies are freed) and drops
   the ones unused for a while. */
int dl_gpu_render(DlGpu *g, SDL_GPUCommandBuffer *cmd, const DlFrame *f, DlTexCache *tc);

/* The output image (640x448 times the scale), valid after dl_gpu_render. */
SDL_GPUTexture *dl_gpu_output(const DlGpu *g, uint32_t *w, uint32_t *h);

/* The host's own picture over the game's (PORT-019's settings panel): 640x448
   RGBA8 with the GS's alpha (0x80 opaque), copied, drawn over the output of
   every frame rendered from now on; NULL takes it away. Never part of a
   frame the Core sent, so nothing of it reaches a GS dump or the Core. */
int dl_gpu_set_overlay(DlGpu *g, const uint8_t *rgba);

/* Render a frame and submit it without waiting (a window's path). */
int dl_gpu_render_submit(DlGpu *g, const DlFrame *f, DlTexCache *tc);

/* Render a frame and read the output back: w * h * 4 bytes, RGBA, rows
   top first. Submits and waits. */
int dl_gpu_render_read(DlGpu *g, const DlFrame *f, DlTexCache *tc, uint8_t *rgba);

#endif
