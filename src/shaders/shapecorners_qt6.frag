uniform sampler2D sampler;       // The painted contents of the window.
varying vec2 texcoord0;          // The XY location of the rendering pixel. Starting from {0.0, 0.0} to {1.0, 1.0}
// Note: This version of GLSL uses the built-in variable `gl_FragColor` instead of `out vec4 fragColor;`

#include "shapecorners.glsl"
#include "colormanagement.glsl"
#include "saturation.glsl"

uniform vec4 modulation;         // This variable is assigned and used by KWinEffects used for proper fading.

void main(void)
{
    vec4 tex = texture2D(sampler, texcoord0);
    tex = run(tex);

    tex = sourceEncodingToNitsInDestinationColorspace(tex);
    tex = adjustSaturation(tex);
    tex *= modulation;

    gl_FragColor = nitsToDestinationEncoding(tex);
}
