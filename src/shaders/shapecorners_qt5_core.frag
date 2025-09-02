#version 140

in vec2 texcoord0;               // The XY location of the rendering pixel. Starting from {0.0, 0.0} to {1.0, 1.0}
out vec4 fragColor;              // The RGBA color that can be rendered

#include "shapecorners.glsl"

uniform vec4 modulation;         // This variable is assigned and used by KWinEffects used for proper fading.
uniform float saturation;        // This variable is assigned and used by KWinEffects used for proper fading.

void main(void)
{
    vec4 tex = texture(sampler, texcoord0);  // The RGBA of the XY pixel for the painted window
    tex = run(texcoord0, tex);

    // Apply the saturation and modulation. This is essential for proper fades in other KWin effects.
    if (saturation != 1.0) {
        vec3 desaturated = tex.rgb * vec3( 0.30, 0.59, 0.11 );
        desaturated = vec3( dot( desaturated, tex.rgb ));
        tex.rgb = tex.rgb * vec3( saturation ) + desaturated * vec3( 1.0 - saturation );
    }
    tex *= modulation;

    // Send to output
    fragColor = tex;
}
