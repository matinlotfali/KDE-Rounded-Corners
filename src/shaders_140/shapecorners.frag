#version 140

uniform sampler2D front;         // The painted contents of the window.
uniform float radius;            // The thickness of the outline in pixels specified in settings.
uniform vec2 windowSize;         // Containing `window->frameGeometry().size()`
uniform vec2 windowExpandedSize; // Containing `window->expandedGeometry().size()`

uniform vec2 windowTopLeft;      /* The distance between the top-left of `expandedGeometry` and
                                  * the top-left of `frameGeometry`. When `windowTopLeft = {0,0}`, it means
                                  * `expandedGeometry = frameGeometry` and there is no shadow. */

uniform vec4 shadowColor;        // The RGBA of the shadow color specified in settings.
uniform float shadowSize;        // The shadow size specified in settings.
uniform vec4 outlineColor;       // The RGBA of the outline color specified in settings.
uniform float outlineThickness;  // The thickness of the outline in pixels specified in settings.

uniform vec4 modulation;         // This variable is assigned and used by KWinEffects used for proper fading.
uniform float saturation;        // This variable is assigned and used by KWinEffects used for proper fading.

in vec2 texcoord0;               // The XY location of the rendering pixel. Starting from {0.0, 0.0} to {1.0, 1.0}
out vec4 fragColor;              // The RGBA color that can be rendered

bool isDrawingShadows() { return  windowSize != windowExpandedSize && shadowColor.a > 0.0; }
bool isDrawingOutline() { return  outlineColor.a > 0.0 && outlineThickness > 0.0; }

/*
 *  \brief This function generates the shadow color based on the distance_from_center
 *  \param distance_from_center: Distance of the rendering point and the reference point that is being used for rounding corners.
 *  \return The RGBA color to be used for shadow.
 */
vec4 shadowColor(float distance_from_center) {
    if(!isDrawingShadows())
        return vec4(0,0,0,0);
    float percent = -distance_from_center/shadowSize + 1;
    if(percent < 0)
        return vec4(0,0,0,0);
    else
        return vec4(shadowColor.rgb * shadowColor.a * percent, shadowColor.a * percent);
}

/*
 *  \brief This function is used to choose the pixel color based on its distance to the center input.
 *  \param coord0: The XY point
 *  \param tex: The RGBA color of the pixel in XY
 *  \param center: The origin XY point that is being used as a reference for rounding corners.
 *  \return The RGBA color to be used instead of tex input.
 */
vec4 shapeCorner(vec2 coord0, vec4 tex, vec2 center) {
    float distance_from_center = distance(coord0, center);
    vec4 c = shadowColor(distance_from_center);

    if(isDrawingOutline()) {
        vec4 outlineOverlay = vec4(mix(tex.rgb, outlineColor.rgb, outlineColor.a), 1.0);

        if (distance_from_center < radius - outlineThickness/2.0) {
            // from the window to the outline
            float antialiasing = clamp(radius-outlineThickness+0.5-distance_from_center, 0.0, 1.0);
            return mix(outlineOverlay, tex, antialiasing);
        }
        else {
            // from the outline to the shadow
            float antialiasing = clamp(distance_from_center-radius+0.5, 0.0, 1.0);
            return mix(outlineOverlay, c, antialiasing);
        }
    }
    else {
        // from the window to the shadow
        float antialiasing = clamp(radius-distance_from_center, 0.0, 1.0);
        return mix(c, tex, antialiasing);
    }
}

void main(void)
{
    vec4 tex = texture(front, texcoord0);  // The RGBA of the XY pixel for the painted window

    /* Since `texcoord0` is ranging from {0.0, 0.0} to {1.0, 1.0} is not pixel intuitive,
     * I am changing the range to become from {0.0, 0.0} to {width, height}
     * in a way that {0,0} is the top-left of the window and not its shadow.
     * This means areas with negative numbers and areas beyond windowSize is considered part of the shadow. */
    vec2 coord0 = vec2(texcoord0.x * windowExpandedSize.x - windowTopLeft.x,
                    (1-texcoord0.y)* windowExpandedSize.y - windowTopLeft.y);

    /*
        Split the window into these sections below. They will have a different center of circle for rounding.

        TL  T   T   TR
        L   x   x   R
        L   x   x   R
        BL  B   B   BR
    */
    if (coord0.y < radius) {
        if (coord0.x < radius)
            tex = shapeCorner(coord0, tex, vec2(radius, radius));                   // Section TL
        else if (coord0.x > windowSize.x - radius)
            tex = shapeCorner(coord0, tex, vec2(windowSize.x - radius, radius));    // Section TR
        else if (coord0.y < outlineThickness)
            tex = shapeCorner(coord0, tex, vec2(coord0.x, radius));                 // Section T
    }
    else if (coord0.y > windowSize.y - radius) {
        if (coord0.x < radius)
            tex = shapeCorner(coord0, tex, vec2(radius, windowSize.y - radius));    // Section BL
        else if (coord0.x > windowSize.x - radius)
            tex = shapeCorner(coord0, tex, vec2(windowSize.x - radius, windowSize.y - radius)); // Section BR
        else if (coord0.y > windowSize.y - outlineThickness)
            tex = shapeCorner(coord0, tex, vec2(coord0.x, windowSize.y - radius));  // Section B
    }
    else {
        if (coord0.x < radius)
            tex = shapeCorner(coord0, tex, vec2(radius, coord0.y));                 // Section L
        else if (coord0.x > windowSize.x - radius)
            tex = shapeCorner(coord0, tex, vec2(windowSize.x - radius, coord0.y));  // Section R

        // For section x, the tex is not changing
    }

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
