// Sampler for the color index map
uniform sampler2D index;

// Sampler for the iteration count map and the lognorm map
uniform sampler2D iter;
uniform sampler2D lognorm;

uniform sampler2D arg;

// Sampler for the texture and the overlay image
uniform sampler2D texture;
uniform sampler2D overlay;

// Sampler for the color palette
uniform sampler2D palette;

// Sampler for the normal map
uniform sampler2D normal;
uniform sampler2D normalRe;
uniform sampler2D normalIm;

// Light direction
uniform vec3 lightDir;

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

    /*
    vec4 normalMap = texture2D(normal, coord);
    float re = (normalMap.g * 256.0 + normalMap.r); // / (256.0 * 255.0);
    float im = (normalMap.a * 256.0 + normalMap.b); //  / (256.0 * 255.0);
     */
    // vec3 N = normalMap * 2.0 - 1.0;

    // float arg = (atan(im, re) + PI) / (2.0 * PI);

    float nrmRe = decode_float(texture2D(normalRe, coord));
    float nrmIm = decode_float(texture2D(normalIm, coord));

    // float arg = decode_float(texture2D(arg, coord));
    float arg = (atan(nrmIm, nrmRe) + PI) / (2.0 * PI);

    float px = mod(arg * 5.0, 1.0);
    float py = mod(sl * 5.0, 1.0);

    return vec3(texture2D(texture, vec2(px,py)).xyz);
//    return vec3(texture2D(texture, vec2(px,py)).xyz);
}

void main()
{
    vec2 coord = gl_TexCoord[0].xy;

    //
    // Adapted from https://github.com/mattdesl/lwjgl-basics/wiki/ShaderLesson6
    //

    // RGBA of our diffuse color
    vec4 diffuseColor =  vec4(deriveColor(coord), 1.0);

    // RGB of our normal map
    vec3 normalMap = texture2D(normal, coord).rgb;

    // Normalize our vectors
    vec3 N = normalize(normalMap * 2.0 - 1.0);
    vec3 L = normalize(lightDir);

    // Compute "N dot L" to determine our diffuse term
    float lambert = max(dot(N, L), 0.0);

    // Modulate diffuse color
    vec3 final;

    if (normalMap.x == 0.0 && normalMap.y == 0.0 && normalMap.z == 0.0) {

        final = diffuseColor.rgb;

    } else {

        float scale = 0.75;
        vec3 hsv = rgb2hsv(diffuseColor.rgb);
        hsv.z *= (lambert * scale) + 1.0 - 0.5 * scale;
        final = hsv2rgb(hsv);
    }

    // Experimental
    float sl = mod(compute_sl(coord) / (2.0 * 3.14159), 1.0);
    final = texture2D(palette, vec2(sl, 0.0)).xyz;

    final = mix(final, compute_tex_pixel(coord), 0.5);

    gl_FragColor = gl_Color * vec4(final, 1.0);
}
