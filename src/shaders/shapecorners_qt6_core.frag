#version 140

uniform sampler2D sampler;       // The painted contents of the window.
in vec2 texcoord0;               // The XY location of the rendering pixel. Starting from {0.0, 0.0} to {1.0, 1.0}
out vec4 fragColor;              // The RGBA color that can be rendered

#include "shapecorners.glsl"
#include "colormanagement.glsl"
#include "saturation.glsl"

uniform vec4 modulation;         // This variable is assigned and used by KWinEffects used for proper fading.

void main(void)
{
    vec4 tex = texture(sampler, texcoord0);
    tex = run(tex);

    tex = sourceEncodingToNitsInDestinationColorspace(tex);
    tex = adjustSaturation(tex);

    // to preserve perceptual contrast, apply the inversion in gamma 2.2 space
    tex = nitsToEncoding(tex, gamma22_EOTF, 0.0, destinationReferenceLuminance);
    tex *= modulation;
    tex.rgb *= tex.a;
    tex = encodingToNits(tex, gamma22_EOTF, 0.0, destinationReferenceLuminance);

    fragColor = nitsToDestinationEncoding(tex);
}
