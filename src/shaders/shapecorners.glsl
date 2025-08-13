uniform float radius;            // The thickness of the outline in pixels specified in settings.
uniform vec2 windowSize;         // Containing `window->frameGeometry().size()`
uniform vec2 windowExpandedSize; // Containing `window->expandedGeometry().size()`

uniform vec2 windowTopLeft;      /* The distance between the top-left of `expandedGeometry` and
                                  * the top-left of `frameGeometry`. When `windowTopLeft = {0,0}`, it means
                                  * `expandedGeometry = frameGeometry` and there is no shadow. */

uniform vec4  outlineColor;           // The RGBA of the outline color specified in settings.
uniform float outlineThickness;       // The thickness of the outline in pixels specified in settings.
uniform vec4  secondOutlineColor;     // The RGBA of the second outline color specified in settings.
uniform float secondOutlineThickness; // The thickness of the second outline in pixels specified in settings.
uniform vec4  outerOutlineColor;      // The RGBA of the outer outline color specified in settings.
uniform float outerOutlineThickness;  // The thickness of the outer outline in pixels specified in settings.

vec2 tex_to_pixel(vec2 texcoord) {
    return vec2(texcoord0.x * windowExpandedSize.x - windowTopLeft.x,
    (1.0 - texcoord0.y) * windowExpandedSize.y - windowTopLeft.y);
}
vec2 pixel_to_tex(vec2 pixelcoord) {
    return vec2((pixelcoord.x + windowTopLeft.x) / windowExpandedSize.x,
    1.0 - (pixelcoord.y + windowTopLeft.y) / windowExpandedSize.y);
}
bool hasExpandedSize() { return windowSize != windowExpandedSize; }
bool hasPrimaryOutline() { return outlineColor.a > 0.0 && outlineThickness > 0.0; }
bool hasSecondOutline() { return secondOutlineColor.a > 0.0 && secondOutlineThickness > 0.0; }
bool hasOuterOutline() { return hasExpandedSize() && outerOutlineColor.a > 0.0 && outerOutlineThickness > 0.0; }

#include "shapecorners_shadows.glsl"

bool is_within(float point, float a, float b) {
    return (point >= min(a, b) && point <= max(a, b));
}
bool is_within(vec2 point, vec2 corner_a, vec2 corner_b) {
    return is_within(point.x, corner_a.x, corner_b.x) && is_within(point.y, corner_a.y, corner_b.y);
}

/*
 *  \brief This function is used to choose the pixel color based on its distance to the center input.
 *  \param coord0: The XY point
 *  \param tex: The RGBA color of the pixel in XY
 *  \param start: The reference XY point to determine the center of the corner roundness.
 *  \param angle: The angle in radians to move away from the start point to determine the center of the corner roundness.
 *  \param is_corner: Boolean to know if its a corner or an edge
 *  \param coord_shadowColor: The RGBA color of the shadow of the pixel behind the window.
 *  \return The RGBA color to be used instead of tex input.
 */
vec4 shapeCorner(vec2 coord0, vec4 tex, vec2 start, float angle, vec4 coord_shadowColor) {
    vec2 angle_vector = vec2(cos(angle), sin(angle));
    float corner_length = (abs(angle_vector.x) < 0.1 || abs(angle_vector.y) < 0.1) ? 1.0 : sqrt(2.0);
    vec2 roundness_center = start + radius * angle_vector * corner_length;
    vec2 outlineStart = start + outlineThickness * angle_vector * corner_length;
    vec2 secondOutlineStart = start + (outlineThickness + secondOutlineThickness) * angle_vector * corner_length;
    vec2 outerOutlineEnd = start - outerOutlineThickness * angle_vector * corner_length;
    float distance_from_center = distance(coord0, roundness_center);

    vec4 outerOutlineOverlay = mix(coord_shadowColor, outerOutlineColor, outerOutlineColor.a);
    if (hasPrimaryOutline() && hasSecondOutline()) {
        vec4 outlineOverlay = vec4(mix(tex.rgb, outlineColor.rgb, outlineColor.a), 1.0);
        vec4 secondOutlineOverlay = vec4(mix(tex.rgb, secondOutlineColor.rgb, secondOutlineColor.a), 1.0);

        if (outlineThickness > radius && is_within(coord0, outlineStart, start) && !is_within(coord0, roundness_center, start)) {
            // when the outline is bigger than the roundness radius
            // from the window to the outline is sharp
            // no antialiasing is needed because it is not round
            return outlineOverlay;
        }
        else if (distance_from_center < radius - outlineThickness + 0.5) {
            // from the window to the outline
            float antialiasing = clamp(radius - outlineThickness + 0.5 - distance_from_center, 0.0, 1.0);
            return mix(outlineOverlay, tex, antialiasing);
        }
        else if (hasOuterOutline()) {

            if (distance_from_center < radius + 0.5) {
                // from the outline to the outer outline
                float antialiasing = clamp(radius + 0.5 - distance_from_center, 0.0, 1.0);
                return mix(outerOutlineOverlay, outlineOverlay, antialiasing);
            }
            else {
                // from the outer outline to the shadow
                if (radius > 0.1) {
                    float antialiasing = clamp(distance_from_center - radius - outerOutlineThickness + 0.5, 0.0, 1.0);
                    return mix(outerOutlineOverlay, coord_shadowColor, antialiasing);
                } else {
                    // when the window is not rounded, we don't need to round the outerary outline
                    // and since it is not rounded, we don't need antialiasing.
                    return is_within(coord0, outlineStart, outerOutlineEnd) ? outerOutlineOverlay : coord_shadowColor;
                }
            }
        } else {
            // from the first outline to the shadow
            float antialiasing = clamp(distance_from_center - radius + 0.5, 0.0, 1.0);
            return mix(outlineOverlay, coord_shadowColor, antialiasing);
        }
    }
    else if (hasOuterOutline()) {
        if (distance_from_center < radius + 0.5) {
            // from window to the outer outline
            float antialiasing = clamp(radius + 0.5 - distance_from_center, 0.0, 1.0);
            return mix(outerOutlineOverlay, tex, antialiasing);
        }
        else {
            // from the outer outline to the shadow
            if (radius > 0.1) {
                float antialiasing = clamp(distance_from_center - radius - outerOutlineThickness + 0.5, 0.0, 1.0);
                return mix(outerOutlineOverlay, coord_shadowColor, antialiasing);
            } else {
                // when the window is not rounded, we don't need to round the outerary outline
                // and since it is not rounded, we don't need antialiasing.
                return is_within(coord0, outlineStart, outerOutlineEnd) ? outerOutlineOverlay : coord_shadowColor;
            }
        }
    }

    // if other conditions don't apply, just don't draw an outline, from the window to the shadow
    float antialiasing = clamp(radius - distance_from_center + 0.5, 0.0, 1.0);
    return mix(coord_shadowColor, tex, antialiasing);
}

vec4 run(vec4 tex) {
    if (tex.a == 0.0) {
        return tex;
    }

    float r = max(radius, outlineThickness);

/* Since `texcoord0` is ranging from {0.0, 0.0} to {1.0, 1.0} is not pixel intuitive,
     * I am changing the range to become from {0.0, 0.0} to {width, height}
     * in a way that {0,0} is the top-left of the window and not its shadow.
     * This means areas with negative numbers and areas beyond windowSize is considered part of the shadow. */
    vec2 coord0 = tex_to_pixel(texcoord0);

    vec4 coord_shadowColor = getShadow(coord0, r, tex);

/*
        Split the window into these sections below. They will have a different center of circle for rounding.

        TL  T   T   TR
        L   x   x   R
        L   x   x   R
        BL  B   B   BR
    */
    if (coord0.y < r) {
        if (coord0.x < r) {
            return shapeCorner(coord0, tex, vec2(0.0, 0.0), radians(45.0), coord_shadowColor);            // Section TL
        } else if (coord0.x > windowSize.x - r) {
            return shapeCorner(coord0, tex, vec2(windowSize.x, 0.0), radians(135.0), coord_shadowColor);   // Section TR
        } else if (coord0.y < outlineThickness) {
            return shapeCorner(coord0, tex, vec2(coord0.x, 0.0), radians(90.0), coord_shadowColor);        // Section T
        }
    }
    else if (coord0.y > windowSize.y - r) {
        if (coord0.x < r) {
            return shapeCorner(coord0, tex, vec2(0.0, windowSize.y), radians(315.0), coord_shadowColor);       // Section BL
        } else if (coord0.x > windowSize.x - r) {
            return shapeCorner(coord0, tex, vec2(windowSize.x, windowSize.y), radians(225.0), coord_shadowColor);// Section BR
        } else if (coord0.y > windowSize.y - outlineThickness) {
            return shapeCorner(coord0, tex, vec2(coord0.x, windowSize.y), radians(270.0), coord_shadowColor);    // Section B
        }
    }
    else {
        if (coord0.x < r) {
            return shapeCorner(coord0, tex, vec2(0.0, coord0.y), radians(0.0), coord_shadowColor);             // Section L
        } else if (coord0.x > windowSize.x - r) {
            return shapeCorner(coord0, tex, vec2(windowSize.x, coord0.y), radians(180.0), coord_shadowColor);  // Section R
        }
        // For section x, the tex is not changing
    }
    return tex;
}

