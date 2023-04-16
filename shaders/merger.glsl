uniform sampler2D current;
uniform sampler2D prev;
uniform sampler2D prevprev;

void main()
{
    vec2 coord = gl_TexCoord[0].xy;
    vec4 pixel = texture2D(current, coord);
    vec4 pixel2 = texture2D(prev, coord);
    vec4 pixel3 = texture2D(prevprev, coord);

    // Experimental: Motion blur (does not work as expected :-( )
    gl_FragColor = mix(mix(pixel2, pixel3, vec4(0.5,0.5,0.5,0.5)), pixel, vec4(0.5,0.5,0.5,0.5));
}
