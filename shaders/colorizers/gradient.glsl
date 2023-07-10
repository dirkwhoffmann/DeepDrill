// Sampler for the normalized iteration count map
uniform sampler2D nitcnt;

// Samplers for the normal map
uniform sampler2D normalRe;
uniform sampler2D normalIm;

// Sampler for the color palette
uniform sampler2D palette;

// Palette adjustments
uniform float paletteScale;
uniform float paletteOffset;

// Sampler for the distance estimate
uniform sampler2D dist;

// Distance estimation adjustments
uniform float distThreshold;

// Sampler for the texture image
uniform sampler2D texture;

// Texture adjustments
uniform float textureOpacity;
uniform float textureScale;
uniform float textureOffset;

// Sampler for the overlay image
uniform sampler2D overlay;


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

// Derives the color for a given coordinate from the color palette
vec3 deriveColor(vec2 coord)
{
    float nic = decode_float(texture2D(nitcnt, coord));
    float px = mod(nic / 64.0 * paletteScale + paletteOffset, 1.0);

    return texture2D(palette, vec2(px, 0.0)).rgb;
}

// Derives the texture pixel for a given coordinate from the texture image
vec4 deriveTexturePixel(vec2 coord, float nrmRe, float nrmIm)
{
    float PI = 3.141592653589793238;
    float nic = decode_float(texture2D(nitcnt, coord));

    float arg = -(atan(nrmIm, nrmRe) + PI) / (2.0 * PI);

    float px = mod(arg * 5.0 * textureScale + textureOffset, 1.0);
    float py = mod(nic * 0.5 * textureScale, 1.0);

    return texture2D(texture, vec2(px,py));
}

void main()
{
    vec2 coord = gl_TexCoord[0].xy;
    coord.y = 1.0 - coord.y;

    // Get diffuse color from palette image
    vec3 diffuseColor = deriveColor(coord);

    // Get the normal vector
    float nrmRe = decode_float(texture2D(normalRe, coord));
    float nrmIm = decode_float(texture2D(normalIm, coord));

    // Get the texture pixel from the texture image
    vec4 textureColor = deriveTexturePixel(coord, nrmRe, nrmIm);

    // Mix diffuse color with the texture color
    diffuseColor = mix(diffuseColor, textureColor.rgb, textureOpacity * textureColor.a);

    // Apply the distance estimator
    float dist = decode_float(texture2D(dist, coord));
    vec3 final = dist < distThreshold ? vec3(0.0,0.0,0.0) : diffuseColor;

    // Superimpose the overlay image
    vec4 ovl = texture2D(overlay, coord);
    final = mix(final, ovl.xyz, ovl.a);

    gl_FragColor = gl_Color * vec4(final, 1.0);
}
