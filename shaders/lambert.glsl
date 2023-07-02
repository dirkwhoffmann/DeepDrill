// Sampler for the color index map
uniform sampler2D index;        // DEPRECATED

// Sampler for the iteration count map and the lognorm map
uniform sampler2D iter;
uniform sampler2D lognorm;

uniform sampler2D arg;          // DEPRECATED

// Sampler for the texture and the overlay image
uniform sampler2D texture;
uniform sampler2D overlay;

// Sampler for the color palette
uniform sampler2D palette;

// Sampler for the normal map
uniform sampler2D normal;       // DEPRECATED
uniform sampler2D normalRe;
uniform sampler2D normalIm;

// Light direction
uniform vec3 lightDir;

// Palette scaling factor
// uniform float paletteStretch;

// Palette offset
// uniform float paletteShift;

// Texture image opacity
// uniform float opacity;

// Texture scaling factor
// uniform float textureStretch;

// Palette offset
// uniform float textureShift;

// See
// https://stackoverflow.com/questions/15095909/from-rgb-to-hsv-in-opengl-glsl
// for a more sophisticated shader

// All components are in the range [0...1]
vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

// All components are in the range [0...1]
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 deriveColor(vec2 coord)
{
    // Read entry from color index table
    vec4 colorIndex = texture2D(index, coord);

    if (colorIndex.a != 0.0) {

        // The RGB value is hard-coded
        return colorIndex.rgb;

    } else {

        // Get the RGB value from the color palette
        coord.x = colorIndex.b + colorIndex.g / 256.0 + colorIndex.r / (256.0 * 256.0);
        return texture2D(palette, coord).xyz;
    }
}

// See
// https://stackoverflow.com/questions/63827836/is-it-possible-to-do-a-rgba-to-float-and-back-round-trip-and-read-the-pixels-in/63830492#63830492
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

float decode_int(vec4 v)
{
    return 256.0 * (v.r + 256.0 * (v.g + 256.0 * (v.b + 256.0 * v.a)));
}

float compute_sl(vec2 coord)
{
    float count = decode_int(texture2D(iter, coord));
    float lnorm = decode_float(texture2D(lognorm, coord));

    return (count - log2(lnorm) + 4.0) * 0.075;
}

vec3 compute_tex_pixel(vec2 coord)
{
    float PI = 3.141592653589793238;
    float sl = compute_sl(coord);

    float nrmRe = decode_float(texture2D(normalRe, coord));
    float nrmIm = decode_float(texture2D(normalIm, coord));

    // float arg = decode_float(texture2D(arg, coord));
    float arg = (atan(nrmIm, nrmRe) + PI) / (2.0 * PI);

    float px = mod(arg * 5.0, 1.0);
    float py = mod(sl * 5.0, 1.0);

    return vec3(texture2D(texture, vec2(px,py)).xyz);
}

void main()
{
    vec2 coord = gl_TexCoord[0].xy;

    //
    // Adapted from https://github.com/mattdesl/lwjgl-basics/wiki/ShaderLesson6
    //

    // Get diffuse color from palette image
    float sl = mod(compute_sl(coord) / (2.0 * 3.14159), 1.0);
    vec4 diffuseColor = texture2D(palette, vec2(sl, 0.0));

    // Mix with texture image
    diffuseColor = mix(diffuseColor, vec4(compute_tex_pixel(coord), 1.0), 0.5);

    // Get normal vector and light vector
    float nrmRe = decode_float(texture2D(normalRe, coord));
    float nrmIm = decode_float(texture2D(normalIm, coord));

    // Modulate diffuse color
    vec3 final;

    if (nrmRe == 0.0 && nrmIm == 0.0) {

        final = diffuseColor.rgb;

    } else {

        vec3 N = normalize(vec3(nrmRe, nrmIm, 1.0));
        vec3 L = normalize(lightDir);
        float lambert = max(dot(N, L), 0.0);

        float lambertScale = 0.75;
        vec3 hsv = rgb2hsv(diffuseColor.rgb);
        hsv.z *= (lambert * lambertScale) + 1.0 - 0.5 * lambertScale;
        final = hsv2rgb(hsv);
    }

    // Superimpose the overlay image
    vec4 ovl = texture2D(overlay, coord);
    final = mix(final.xyz, ovl.xyz, ovl.a);

    gl_FragColor = gl_Color * vec4(final, 1.0);
}
