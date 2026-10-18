#version 140

in vec2  texcoord0; // The XY location of the rendering pixel. Starting from {0.0, 0.0} to {1.0, 1.0}
out vec4 fragColor; // The RGBA color that can be rendered

// KWin passes `position0` only to shaders generated with the RoundedCorners trait.
#ifdef TRAIT_ROUNDED_CORNERS
#if TRAIT_ROUNDED_CORNERS && (__VERSION__ >= 300 || defined(GL_OES_standard_derivatives))
in vec2 position0; // The vertex position relative to the window's top-left, growing downward.
#define HAS_POSITION0
#endif
#endif

#include "colormanagement.glsl"
#include "saturation.glsl"
#include "shapecorners.glsl"

uniform vec4 modulation; // This variable is assigned and used by KWinEffects used for proper fading.

void main(void)
{
#ifdef HAS_POSITION0
    yInverted = detect_y_inverted();
#endif
    vec4 tex = texture(sampler, texcoord0);

    tex = run(texcoord0, tex);

    tex = sourceEncodingToNitsInDestinationColorspace(tex);
    tex = adjustSaturation(tex);
    tex *= modulation;

    fragColor = nitsToDestinationEncoding(tex);
}
