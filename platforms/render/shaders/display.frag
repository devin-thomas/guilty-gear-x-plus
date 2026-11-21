#version 450
// The display stage (PORT-009): the GS's two read circuits and its merge
// circuit, as PMODE, DISPFB, DISPLAY and BGCOLOR set them (dl_frame.h,
// DlDisplay; dl_gpu.c fills the uniforms).
//
// Each circuit shows a surface from (DBX, DBY) inside its own rectangle of
// the output. The merge starts from BGCOLOR, puts circuit 2 over it unless
// SLBG asks for the background alone, then blends circuit 1 over that at
// ALP (MMOD 1) or at its own alpha (MMOD 0), as PCSX2's merge does. The
// game shows one buffer through both circuits, the second a line lower, at
// ALP 0x7F, so every output line mixes with the line above it.

layout(location = 0) out vec4 o_col;

layout(set = 2, binding = 0) uniform sampler2D fb1;   // circuit 1's surface
layout(set = 2, binding = 1) uniform sampler2D fb2;   // circuit 2's

layout(set = 3, binding = 0) uniform DisplayPS {
    ivec4 rect[2];    // each circuit's picture on the output: x, y, w, h (w 0: off)
    ivec4 src[2];     // the surface pixel it starts at: DBX, DBY
    vec4 step[2];     // surface pixels per output pixel, across and down
    vec4 bg;          // BGCOLOR
    ivec4 mode;       // MMOD, SLBG, ALP
    ivec4 fb_size;    // the surfaces' size
};

// Circuit i's pixel at output pixel p, or false outside its picture. A read
// past the surface is black.
bool circuit(int i, ivec2 p, out vec4 c)
{
    ivec2 d = p - rect[i].xy, q;

    c = vec4(0.0);
    if (rect[i].z == 0 || any(lessThan(d, ivec2(0))) || any(greaterThanEqual(d, rect[i].zw))) {
        return false;
    }
    q = src[i].xy + ivec2(floor(vec2(d) * step[i].xy));
    if (all(greaterThanEqual(q, ivec2(0))) && all(lessThan(q, fb_size.xy))) {
        c = i == 0 ? texelFetch(fb1, q, 0) : texelFetch(fb2, q, 0);
    }
    return true;
}

void main()
{
    ivec2 p = ivec2(gl_FragCoord.xy);
    vec4 c1, c2, b = bg;

    if (circuit(1, p, c2) && mode.y == 0) {
        b = c2;
    }
    if (circuit(0, p, c1)) {
        float a = mode.x != 0 ? float(mode.z) / 255.0 : c1.a;

        b = c1 * a + b * (1.0 - a);
    }
    // rounded here, to the nearest of the 8-bit values, so no backend's own
    // float-to-UNORM rounding decides it
    o_col = vec4(floor(b.rgb * 255.0 + 0.5) / 255.0, 1.0);
}
