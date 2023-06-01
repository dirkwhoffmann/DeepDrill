// Sampler for the color image
uniform sampler2D image;

// Sampler for the normal map
uniform sampler2D normal;

// Texture size
uniform vec2 size;

// Light position (normalized)
uniform vec3 lightPos;

// Light color (alpha = intensity)
uniform vec4 lightColor;

// Ambient light color (alpha = intensity)
uniform vec4 ambientColor;

// Attenuation coefficients
uniform vec3 falloff;

void main()
{
    vec2 coord = gl_TexCoord[0].xy;
    vec4 pixel1 = texture2D(image, coord);

    // TODO: Lambert shading

    gl_FragColor = gl_Color * pixel1;
}
