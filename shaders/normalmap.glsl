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
    vec3 normalMap = texture2D(normal, coord).rgb;
    gl_FragColor = gl_Color * vec4(normalMap, 1.0);
}
