// Sampler for the current texture
uniform sampler2D curr;

// Sampler for the next texture
uniform sampler2D next;

// Texture size
uniform vec2 size;

// Zoom factor
uniform float zoom;

// Normalized inbetween [0;1)
uniform float frame;

vec2 zoomed(vec2 coord)
{
    return (coord / zoom) + 0.5 - (0.5 / zoom);
}

vec4 bilinear(sampler2D sampler, vec2 coord)
{
    float dx = 1.0 / size.x;
    float dy = 1.0 / size.y;

    vec4 p0q0 = texture2D(sampler, coord);
    vec4 p1q0 = texture2D(sampler, coord + vec2(dx, 0));
    vec4 p0q1 = texture2D(sampler, coord + vec2(0, dy));
    vec4 p1q1 = texture2D(sampler, coord + vec2(dx,dy));

    float a = fract(coord.x * size.x);
    vec4 pInterp_q0 = mix(p0q0, p1q0, a);
    vec4 pInterp_q1 = mix(p0q1, p1q1, a);

    float b = fract(coord.y * size.y);
    vec4 pInterp = mix(pInterp_q0, pInterp_q1, b);

    return pInterp;
}

void main()
{
    // Read texel from the current texture
    vec2 coord = zoomed(gl_TexCoord[0].xy);
    vec4 color1 = bilinear(curr, coord);

    // Check if a corresponding texel exists in the next texture
    vec2 coord2 = 2.0 * coord - vec2(0.5,0.5);
    if (coord2.x >= 0.0 && coord2.x <= 1.0 && coord2.y >= 0.0 && coord2.y <= 1.0) {

        // Read the corresponding texel from the next texture
        vec4 color2 = bilinear(next, coord2);

        // Interpolate between both texels
        color1 = mix(color1, color2, frame);
    }

    gl_FragColor = gl_Color * color1;
}
