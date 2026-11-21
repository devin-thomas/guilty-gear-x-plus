#version 450
// The Draw Layer's primitives (PORT-008): framebuffer pixels to clip space.
// Bindings follow SDL_GPU's layout (build_shaders.py).

layout(location = 0) in vec2 in_pos;    // framebuffer pixels at 1x
layout(location = 1) in vec2 in_uv;     // texels
layout(location = 2) in vec4 in_col;    // RGBAQ bytes / 255

layout(location = 0) out vec4 v_col;
layout(location = 1) out vec2 v_uv;

layout(set = 1, binding = 0) uniform DrawVS {
    vec2 inv_size;    // 2 / (640, 448)
    vec2 pad;
};

void main()
{
    gl_Position = vec4(in_pos.x * inv_size.x - 1.0, 1.0 - in_pos.y * inv_size.y, 0.0, 1.0);
    v_uv = in_uv;
    v_col = in_col;
}
