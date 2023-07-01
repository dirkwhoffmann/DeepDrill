// Sampler for the color index map
uniform sampler2D index;

// Sampler for the color palette
uniform sampler2D palette;

// Sampler for the normal map
uniform sampler2D normal;

// Light direction
uniform vec3 lightDir;

// See
// https://stackoverflow.com/questions/15095909/from-rgb-to-hsv-in-opengl-glsl
// for a more sophisticated shader

// All components are in the range [0...1]
vec3 rgb2hsv(vec3 c)
{
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

// All components are in the range [0...1]
vec3 hsv2rgb(vec3 c)
{
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 deriveColor(vec2 coord)
{
    // Read entry from color index table
    vec4 colorIndex = texture2D(index, coord);

    if (colorIndex.a != 0.0) {

        // The RGB value is hard-coded
        return colorIndex.rgb;

    } else {

        // Get the RGB value from the color palette
        coord.x = colorIndex.b + colorIndex.g / 256.0 + colorIndex.r / (256.0 * 256.0);
        return texture2D(palette, coord).xyz;
    }
}

void main()
{
    vec2 coord = gl_TexCoord[0].xy;

    //
    // Adapted from https://github.com/mattdesl/lwjgl-basics/wiki/ShaderLesson6
    //

    // RGBA of our diffuse color
    vec4 diffuseColor =  vec4(deriveColor(coord), 1.0);

    // RGB of our normal map
    vec3 normalMap = texture2D(normal, coord).rgb;

    // Normalize our vectors
    vec3 N = normalize(normalMap * 2.0 - 1.0);
    vec3 L = normalize(lightDir);

    // Compute "N dot L" to determine our diffuse term
    float lambert = max(dot(N, L), 0.0);

    // Modulate diffuse color
    vec3 final;

    if (normalMap.x == 0.0 && normalMap.y == 0.0 && normalMap.z == 0.0) {

        final = diffuseColor.rgb;

    } else {

        float scale = 0.75;
        vec3 hsv = rgb2hsv(diffuseColor.rgb);
        hsv.z *= (lambert * scale) + 1.0 - 0.5 * scale;
        final = hsv2rgb(hsv);
    }

    gl_FragColor = gl_Color * vec4(final, 1.0);
}
