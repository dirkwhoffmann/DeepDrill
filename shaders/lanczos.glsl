// Adapted from https://invent.kde.org/-/snippets/1736

#version 120

/*
 Hyllian's lanczos windowed-jinc 2-lobe sharper 3D with anti-ringing Shader

 Copyright (C) 2011/2016 Hyllian - sergiogdb@gmail.com

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.

 */

uniform sampler2D curr;
uniform vec2 size;

/*
uniform float lanczos2_window_sinc;
uniform float lanczos2_sinc;
uniform float lanczos2_ar_strength;
uniform float lanczos2_res;
*/
float lanczos2_window_sinc = 0.4;
float lanczos2_sinc = 1.0;
float lanczos2_ar_strength = 0.65;
float lanczos2_res = 0.98;

// varying vec2 qt_TexCoord0;

// A=0.5, B=0.825 is the best jinc approximation for x<2.5. if B=1.0, it's a lanczos filter.
// Increase A to get more blur. Decrease it to get a sharper picture.
// B = 0.825 to get rid of dithering. Increase B to get a fine sharpness, though dithering returns.

#define wa (lanczos2_window_sinc * pi)
#define wb (lanczos2_sinc * pi)

const float halfpi = 1.5707963267948966192313216916398;
const float pi = 3.1415926535897932384626433832795;
const vec3 dtt = vec3(65536.0, 255.0, 1.0);

vec4 reduce4(vec3 A, vec3 B, vec3 C, vec3 D)
{
    return dtt * mat4x3(A, B, C, D);
}

// Calculates the distance between two points
float d(vec2 pt1, vec2 pt2)
{
    vec2 v = pt2 - pt1;
    return sqrt(dot(v, v));
}

vec3 min4(vec3 a, vec3 b, vec3 c, vec3 d)
{
    return min(a, min(b, min(c, d)));
}

vec3 max4(vec3 a, vec3 b, vec3 c, vec3 d)
{
    return max(a, max(b, max(c, d)));
}

vec4 resampler(vec4 x)
{
    return (x == vec4(0.0)) ?  vec4(wa * wb) : sin(x * wa) * sin(x * wb) / (x * x);
}

void main()
{
    vec3 color;
    vec3 E;

    vec2 qt_TexCoord0 = gl_TexCoord[0].xy;

    vec2 dx = vec2(1.0, 0.0);
    vec2 dy = vec2(0.0, 1.0);

    vec2 pc = qt_TexCoord0 * size / lanczos2_res;
    vec2 tex = (floor(pc) + vec2(0.5, 0.5)) * lanczos2_res / size;

    vec2 tc = (floor(pc-vec2(0.5,0.5))+vec2(0.5,0.5));

    mat4 weights;
    weights[0] = resampler(vec4(d(pc, tc    -dx    -dy), d(pc, tc         -dy), d(pc, tc    +dx    -dy), d(pc, tc+2.0*dx    -dy)));
    weights[1] = resampler(vec4(d(pc, tc    -dx     ), d(pc, tc          ), d(pc, tc    +dx     ), d(pc, tc+2.0*dx     )));
    weights[2] = resampler(vec4(d(pc, tc    -dx    +dy), d(pc, tc         +dy), d(pc, tc    +dx    +dy), d(pc, tc+2.0*dx    +dy)));
    weights[3] = resampler(vec4(d(pc, tc    -dx+2.0*dy), d(pc, tc     +2.0*dy), d(pc, tc    +dx+2.0*dy), d(pc, tc+2.0*dx+2.0*dy)));

    dx = dx * lanczos2_res / size;
    dy = dy * lanczos2_res / size;
    tc = tc * lanczos2_res / size;

    // reading the texels

    vec3 c00 = texture2D(curr, tc    -dx    -dy).xyz;
    vec3 c10 = texture2D(curr, tc         -dy).xyz;
    vec3 c20 = texture2D(curr, tc    +dx    -dy).xyz;
    vec3 c30 = texture2D(curr, tc+2.0*dx    -dy).xyz;
    vec3 c01 = texture2D(curr, tc    -dx     ).xyz;
    vec3 c11 = texture2D(curr, tc          ).xyz;
    vec3 c21 = texture2D(curr, tc    +dx     ).xyz;
    vec3 c31 = texture2D(curr, tc+2.0*dx     ).xyz;
    vec3 c02 = texture2D(curr, tc    -dx    +dy).xyz;
    vec3 c12 = texture2D(curr, tc         +dy).xyz;
    vec3 c22 = texture2D(curr, tc    +dx    +dy).xyz;
    vec3 c32 = texture2D(curr, tc+2.0*dx    +dy).xyz;
    vec3 c03 = texture2D(curr, tc    -dx+2.0*dy).xyz;
    vec3 c13 = texture2D(curr, tc     +2.0*dy).xyz;
    vec3 c23 = texture2D(curr, tc    +dx+2.0*dy).xyz;
    vec3 c33 = texture2D(curr, tc+2.0*dx+2.0*dy).xyz;

    color = E = texture2D(curr, qt_TexCoord0).xyz;

    vec3 F6 = texture2D(curr, tex +dx+0.25*dx+0.25*dy).xyz;
    vec3 F7 = texture2D(curr, tex +dx+0.25*dx-0.25*dy).xyz;
    vec3 F8 = texture2D(curr, tex +dx-0.25*dx-0.25*dy).xyz;
    vec3 F9 = texture2D(curr, tex +dx-0.25*dx+0.25*dy).xyz;

    vec3 H6 = texture2D(curr, tex +0.25*dx+0.25*dy+dy).xyz;
    vec3 H7 = texture2D(curr, tex +0.25*dx-0.25*dy+dy).xyz;
    vec3 H8 = texture2D(curr, tex -0.25*dx-0.25*dy+dy).xyz;
    vec3 H9 = texture2D(curr, tex -0.25*dx+0.25*dy+dy).xyz;

    vec4 f0 = reduce4(F6, F7, F8, F9);
    vec4 h0 = reduce4(H6, H7, H8, H9);

    //  Get min/max samples
    vec3 min_sample = min4(c11, c21, c12, c22);
    vec3 max_sample = max4(c11, c21, c12, c22);

    color = weights[0] * transpose(mat4x3(c00, c10, c20, c30));
    color += weights[1] * transpose(mat4x3(c01, c11, c21, c31));
    color += weights[2] * transpose(mat4x3(c02, c12, c22, c32));
    color += weights[3] * transpose(mat4x3(c03, c13, c23, c33));
    color = color / dot(vec4(1.0) * weights, vec4(1.0));

    // Anti-ringing
    vec3 aux = color;
    color = clamp(color, min_sample, max_sample);

    color = mix(aux, color, lanczos2_ar_strength);

    float alpha = texture2D(texture, qt_TexCoord0).a;
    gl_FragColor = vec4(color, alpha);
}
