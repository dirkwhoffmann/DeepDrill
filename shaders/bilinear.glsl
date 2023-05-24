uniform sampler2D texture;
uniform sampler2D texture2;
uniform vec2 texture_size;
uniform float frame;

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
    vec4 color1 = bilinear(texture, coord);

    vec2 coord2 = 2.0 * coord - vec2(0.5,0.5);
    if (coord2.x >= 0.0 && coord2.x <= 1.0 && coord2.y >= 0.0 && coord2.y <= 1.0) {

        vec4 color2 = bilinear(texture2, coord2);
        gl_FragColor = gl_Color * mix(color1, color2, frame);

    } else {

        gl_FragColor = gl_Color * color1;
    }
}
