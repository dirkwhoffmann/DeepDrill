uniform sampler2D texture;

void main()
{
    vec2 coord = gl_TexCoord[0].xy;
    vec4 pixel = texture2D(texture, coord);
    gl_FragColor = gl_Color * pixel;
    // gl_FragColor += vec4(0.5,0.5,0.5,0.5);
}
