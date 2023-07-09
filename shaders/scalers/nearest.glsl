uniform sampler2D curr;

// Zoom factor
uniform float zoom;

vec2 zoomed(vec2 coord)
{
    return (coord / zoom) + 0.5 - (0.5 / zoom);
}

void main()
{
    vec2 coord = gl_TexCoord[0].xy;
    coord.y = 1.0 - coord.y;
    coord = zoomed(coord);

    vec4 pixel = texture2D(curr, coord);
    gl_FragColor = gl_Color * pixel;
}
