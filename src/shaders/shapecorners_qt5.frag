precision mediump float;

uniform sampler2D sampler;       // The painted contents of the window.
varying vec2 texcoord0;          // The XY location of the rendering pixel. Starting from {0.0, 0.0} to {1.0, 1.0}
// Note: This version of GLSL uses the built-in variable `gl_FragColor` instead of `out vec4 fragColor;`

#include "shapecorners.glsl"

uniform vec4 modulation;         // This variable is assigned and used by KWinEffects used for proper fading.
uniform float saturation;        // This variable is assigned and used by KWinEffects used for proper fading.

void main(void)
{
    vec4 tex = texture2D(sampler, texcoord0);
    tex = run(tex);

    // Apply the saturation and modulation. This is essential for proper fades in other KWin effects.
    if (saturation != 1.0) {
        vec3 desaturated = tex.rgb * vec3( 0.30, 0.59, 0.11 );
        desaturated = vec3( dot( desaturated, tex.rgb ));
        tex.rgb = tex.rgb * vec3( saturation ) + desaturated * vec3( 1.0 - saturation );
    }
    tex *= modulation;

    // Send to output
    gl_FragColor = tex;
}
