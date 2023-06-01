uniform sampler2D image;
uniform sampler2D normal;

void main()
{
    vec2 coord = gl_TexCoord[0].xy;
    vec4 pixel1 = texture2D(image, coord);

    // TODO: Lambert shading

    gl_FragColor = gl_Color * pixel1;
}
