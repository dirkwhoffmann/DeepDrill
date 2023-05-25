uniform sampler2D curr;

void main()
{
    vec2 coord = gl_TexCoord[0].xy;
    vec4 pixel = texture2D(curr, coord);
    gl_FragColor = gl_Color * pixel;
}
