uniform sampler2D current;
uniform sampler2D prev;
uniform sampler2D prevprev;

void main()
{
    vec2 coord = gl_TexCoord[0].xy;
    vec4 pixel1 = texture2D(current, coord);
    vec4 pixel2 = texture2D(prev, coord);
    vec4 pixel3 = texture2D(prevprev, coord);
    // gl_FragColor = gl_Color * mix(pixel1, mix(pixel2, pixel3, 0.3), 0.3);
    gl_FragColor = gl_Color * pixel1;
}
