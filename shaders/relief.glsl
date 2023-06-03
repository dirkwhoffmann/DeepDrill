
// Sampler for the color image
uniform sampler2D image;

// Sampler for the normal map
uniform sampler2D normal;

// Texture size
uniform vec2 size;

// Light direction
uniform vec3 lightDir;

void main()
{
    vec2 coord = gl_TexCoord[0].xy;

    // RGB of our normal map
    vec3 normalMap = texture2D(normal, coord).rgb;

    // Normalize our vectors
    vec3 N = normalize(normalMap * 2.0 - 1.0);
    vec3 L = normalize(lightDir);

    // Do Lambert
    float lambert = max(dot(N, L), 0.0);

    // Modulate
    vec3 final = vec3(lambert, lambert, lambert);
    gl_FragColor = gl_Color * vec4(final, 1.0);
}
