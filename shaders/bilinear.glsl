uniform sampler2D texture;
uniform sampler2D texture2;
uniform vec2 texture_size;
uniform float frame;

// from https://stackoverflow.com/questions/13501081/efficient-bicubic-filtering-code-in-glsl
vec4 cubic(float v){
    vec4 n = vec4(1.0, 2.0, 3.0, 4.0) - v;
    vec4 s = n * n * n;
    float x = s.x;
    float y = s.y - 4.0 * s.x;
    float z = s.z - 4.0 * s.y + 6.0 * s.x;
    float w = 6.0 - x - y - z;
    return vec4(x, y, z, w) * (1.0/6.0);
}

vec4 bicubic(sampler2D sampler, vec2 texCoords){

    // vec2 texSize = textureSize(sampler, 0);
    vec2 texSize = texture_size;
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

    return mix(
               mix(sample3, sample2, sx), mix(sample1, sample0, sx)
               , sy);
}


vec4 bilinear(sampler2D sampler, vec2 coord)
{
    float dx = 1.0 / texture_size.x;
    float dy = 1.0 / texture_size.y;

    vec4 p0q0 = texture2D(sampler, coord);
    vec4 p1q0 = texture2D(sampler, coord + vec2(dx, 0));
    vec4 p0q1 = texture2D(sampler, coord + vec2(0, dy));
    vec4 p1q1 = texture2D(sampler, coord + vec2(dx,dy));

    float a = fract(coord.x * texture_size.x);
    vec4 pInterp_q0 = mix(p0q0, p1q0, a);
    vec4 pInterp_q1 = mix(p0q1, p1q1, a);

    float b = fract(coord.y * texture_size.y);
    vec4 pInterp = mix(pInterp_q0, pInterp_q1, b);

    return pInterp;
}

void main()
{
    vec2 coord = gl_TexCoord[0].xy;
    // vec4 color1 = bilinear(texture, coord);
    vec4 color1 = bicubic(texture, coord);

    vec2 coord2 = 2.0 * coord - vec2(0.5,0.5);
    if (coord2.x >= 0.0 && coord2.x <= 1.0 && coord2.y >= 0.0 && coord2.y <= 1.0) {

        // vec4 color2 = bilinear(texture2, coord2);
        vec4 color2 = bicubic(texture2, coord2);
        // gl_FragColor = gl_Color * color1;
        gl_FragColor = gl_Color * mix(color1, color2, frame);

    } else {

        gl_FragColor = gl_Color * color1;
    }
}
