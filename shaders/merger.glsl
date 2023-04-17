uniform sampler2D current;
uniform sampler2D prev;
uniform sampler2D prevprev;

void main()
{
    vec2 coord = gl_TexCoord[0].xy;
    vec4 pixel = texture2D(current, coord);
    gl_FragColor = gl_Color * pixel;
}
