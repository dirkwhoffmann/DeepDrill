// Texture size
uniform vec2 size;

// Sampler for the color image
uniform sampler2D image;

// Sampler for the normal map
uniform sampler2D normal;

// Light direction
uniform vec3 lightDir;

vec4 cubic(float v)
{
    vec4 n = vec4(1.0, 2.0, 3.0, 4.0) - v;
    vec4 s = n * n * n;
    float x = s.x;
    float y = s.y - 4.0 * s.x;
    float z = s.z - 4.0 * s.y + 6.0 * s.x;
    float w = 6.0 - x - y - z;
    return vec4(x, y, z, w) * (1.0/6.0);
}

vec4 bicubic(sampler2D sampler, vec2 texCoords)
{
    vec2 texSize = size;
    vec2 invTexSize = 1.0 / texSize;

    texCoords = texCoords * texSize - 0.5;

    vec2 fxy = fract(texCoords);
    texCoords -= fxy;

    vec4 xcubic = cubic(fxy.x);
    vec4 ycubic = cubic(fxy.y);

    vec4 c = texCoords.xxyy + vec2 (-0.5, +1.5).xyxy;

    vec4 s = vec4(xcubic.xz + xcubic.yw, ycubic.xz + ycubic.yw);
    vec4 offset = c + vec4 (xcubic.yw, ycubic.yw) / s;

    offset *= invTexSize.xxyy;

    vec4 sample0 = texture2D(sampler, offset.xz);
    vec4 sample1 = texture2D(sampler, offset.yz);
    vec4 sample2 = texture2D(sampler, offset.xw);
    vec4 sample3 = texture2D(sampler, offset.yw);

    float sx = s.x / (s.x + s.y);
    float sy = s.z / (s.z + s.w);

    return mix(mix(sample3, sample2, sx), mix(sample1, sample0, sx), sy);
}

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

void main()
{
    vec2 coord = gl_TexCoord[0].xy;

    //
    // Adapted from https://github.com/mattdesl/lwjgl-basics/wiki/ShaderLesson6
    //

    // RGBA of our diffuse color
    vec4 diffuseColor = bicubic(image, coord);
    // vec4 diffuseColor = texture2D(image, coord);

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

        float scale = 1.0;
        vec3 hsv = rgb2hsv(diffuseColor.rgb);
        hsv.z *= (lambert * scale) + 1.0 - 0.5 * scale;
        final = hsv2rgb(hsv);
    }

    gl_FragColor = gl_Color * vec4(final, 1.0);
}
