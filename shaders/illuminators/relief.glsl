// Samplers for the normal map
uniform sampler2D normalRe;
uniform sampler2D normalIm;

// Light direction
uniform vec3 lightDir;

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

vec3 makeSpatial(float nrmRe, float nrmIm)
{
    if (nrmRe == 0.0 && nrmIm == 0.0) {

        return vec3(0.0,0.0,0.0);

    } else {

        vec3 N = normalize(vec3(nrmRe, nrmIm, 1.0));
        vec3 L = normalize(lightDir);
        float lambert = max(dot(N, L), 0.0);

        float lambertScale = 0.65;
        lambert = (lambert * lambertScale) + 0.5 * lambertScale;
        return vec3(lambert, lambert, lambert);
    }
}

void main()
{
    vec2 coord = gl_TexCoord[0].xy;

    // Get the normal vector
    float nrmRe = decode_float(texture2D(normalRe, coord));
    float nrmIm = decode_float(texture2D(normalIm, coord));

    // Normalize
    vec3 N = normalize(vec3(nrmRe, nrmIm, 1.0));
    vec3 L = normalize(lightDir);

    // Apply 3D effect
    vec3 final = makeSpatial(nrmRe, nrmIm);

    gl_FragColor = gl_Color * vec4(final, 1.0);
}
