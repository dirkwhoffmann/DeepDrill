// Sampler for the color palette
uniform sampler2D palette;

// Palette adjustments
uniform float paletteScale;
uniform float paletteOffset;

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
vec3 deriveColor(vec2 coord, float d)
{
    // float dd = 1.0 / (d / 500.0 + 1.0 - distThreshold);
    // float dd = 0.2 * log(1.0 + d);
    // float dd = sqrt(d / 64.0);
    float dd = 1.0 / (d / 2.0 + 1.0);
    // float dd = 1.0 / (sqrt(d) + 1.0);
    float px = mod(dd * paletteScale + paletteOffset, 1.0);

    return texture2D(palette, vec2(px, 0.0)).rgb;
}

void main()
{
    vec2 coord = gl_TexCoord[0].xy;
    coord.y = 1.0 - coord.y;

    // Get the distance estimation
    float d = decode_float(texture2D(dist, coord));
    float alpha;

    // Compute color
    vec3 color = deriveColor(coord, d);

    if (d < distThreshold) {

        float c = 0.0;
        color = vec3(c,c,c);
        alpha = 1.0;

    } else {

        color = vec3(1.0,1.0,1.0);
        alpha = 0.0;
    }


    // Superimpose the overlay image
    vec4 ovl = texture2D(overlay, coord);
    color = mix(color, ovl.xyz, ovl.a);

    gl_FragColor = gl_Color * vec4(color, alpha);
}
