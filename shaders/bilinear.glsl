uniform sampler2D texture;
uniform vec2 texture_size;

void main()
{
    float dx = 1.0 / texture_size.x;
    float dy = 1.0 / texture_size.y;

    vec2 coord = gl_TexCoord[0].xy;
    vec4 p0q0 = texture2D(texture, coord);
    vec4 p1q0 = texture2D(texture, coord + vec2(dx, 0));
    vec4 p0q1 = texture2D(texture, coord + vec2(0, dy));
    vec4 p1q1 = texture2D(texture, coord + vec2(dx,dy));

    float a = fract(coord.x * texture_size.x);
    vec4 pInterp_q0 = mix(p0q0, p1q0, a);
    vec4 pInterp_q1 = mix(p0q1, p1q1, a);

    float b = fract(coord.y * texture_size.y);
    vec4 pInterp = mix(pInterp_q0, pInterp_q1, b);

    gl_FragColor = gl_Color * pInterp;
}
