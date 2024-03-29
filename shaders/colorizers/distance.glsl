// Sampler for the color palette
uniform sampler2D palette;

// Palette adjustments
uniform float paletteScale;
uniform float paletteOffset;

// Background color (color of the Mandelbrot set)
uniform vec4 bgcolor;

// Sampler for the distance estimate
uniform sampler2D dist;

// Distance estimation adjustments
uniform float distThreshold;

// Sampler for the overlay image
uniform sampler2D overlay;

// Decodes a float from a texel
float decode_float(vec4 v)
{
    vec4 bits = v * 255.0;
    float sign = mix(-1.0, 1.0, step(bits[3], 128.0));
    float expo = floor(mod(bits[3] + 0.1, 128.0)) * 2.0 +
    floor((bits[2] + 0.1) / 128.0) - 127.0;
    float sig = bits[0] +
    bits[1] * 256.0 +
    floor(mod(bits[2] + 0.1, 128.0)) * 256.0 * 256.0;
    return sign * (1.0 + sig / 8388607.0) * pow(2.0, expo);
}

// Derives the color for a given coordinate from the color palette
vec4 deriveColor(vec2 coord, float d)
{
    // float dd = 1.0 / (d + 1.0);
    float dd = 0.0;
    float px = mod(dd * paletteScale + paletteOffset, 1.0);

    return texture2D(palette, vec2(px, 0.0));
}

/*
// Converts a RGB value into a HSV value (all components must be in [0...1])
vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

// Converts a HSV value into a RGB value (all components must be in [0...1])
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
*/

void main()
{
    vec2 coord = gl_TexCoord[0].xy;
    coord.y = 1.0 - coord.y;

    // Get the distance estimation
    float d = decode_float(texture2D(dist, coord));

    // Compute color
    vec4 color = deriveColor(coord, d);

    // Apply border effect
    if (d < distThreshold) color = bgcolor;

    // Superimpose the overlay image
    vec4 ovl = texture2D(overlay, coord);
    color.rgb = mix(color.rgb, ovl.xyz, ovl.a);

    gl_FragColor = gl_Color * color;
}
