// Sampler for the texture to process
uniform sampler2D image;

// Samplers for the normal map
uniform sampler2D normalRe;
uniform sampler2D normalIm;

// Light direction
uniform vec3 lightDir;


//
// Utility functions
//

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

// Decodes an u32 integer from a texel
float decode_int(vec4 v)
{
    return 256.0 * (v.r + 256.0 * (v.g + 256.0 * (v.b + 256.0 * v.a)));
}

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

//
//
//

// Applies a lighting effect
vec3 makeSpatial(vec3 color, float nrmRe, float nrmIm)
{
    if (nrmRe == 0.0 && nrmIm == 0.0) {

        return color;

    } else {

        vec3 N = normalize(vec3(nrmRe, nrmIm, 1.0));
        vec3 L = normalize(lightDir);
        float lambert = max(dot(N, L), 0.0);

        float lambertScale = 0.75;
        vec3 hsv = rgb2hsv(color);
        hsv.z *= (lambert * lambertScale) + 1.0 - 0.5 * lambertScale;
        return hsv2rgb(hsv);
    }
}

void main()
{
    vec2 coord = gl_TexCoord[0].xy;
    coord.y = 1.0 - coord.y;

    // Get color pixel
    vec3 diffuseColor = texture2D(image, coord).rgb;

    // Get the normal vector
    float nrmRe = decode_float(texture2D(normalRe, coord));
    float nrmIm = decode_float(texture2D(normalIm, coord));

    // Apply 3D effect
    vec3 final = makeSpatial(diffuseColor, nrmRe, nrmIm);

    gl_FragColor = gl_Color * vec4(final, 1.0);
}
