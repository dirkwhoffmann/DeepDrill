// Sampler for the current texture
uniform sampler2D curr;

// Sampler for the next texture
uniform sampler2D next;

// Texture size
uniform vec2 size;

// Normalized inbetween [0;1)
uniform float frame;

//
// From https://stackoverflow.com/questions/13501081/efficient-bicubic-filtering-code-in-glsl
//

vec2 zoom(vec2 coord)
{
    float s = 1.0 / (1.0 + frame);
    return s * coord + (0.5 - (0.5 * s));
}

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

void main()
{
    vec2 coord = zoom(gl_TexCoord[0].xy);
    vec4 color1 = bicubic(curr, coord);
    gl_FragColor = gl_Color * color1;
}

