uniform sampler2D texture;
uniform vec2 texture_size;

void main()
{
    vec2 dx = vec2(1.0,0.0) / texture_size.x;
    vec2 dy = vec2(0.0,1.0) / texture_size.y;

    vec2 coord = gl_TexCoord[0].xy;
    vec4 pixel = texture2D(texture, coord);
    gl_FragColor = gl_Color * pixel;
}
