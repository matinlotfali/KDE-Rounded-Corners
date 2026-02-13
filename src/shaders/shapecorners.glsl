#include "shapecorners_shadows.glsl"

bool is_within(float point, float a, float b) { return (point >= min(a, b) && point <= max(a, b)); }
bool is_within(vec2 point, vec2 corner_a, vec2 corner_b)
{
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
vec4 shapeCorner(vec2 coord0, vec4 tex, vec2 start, float angle, vec4 coord_shadowColor)
{
    vec2  angle_vector         = vec2(cos(angle), sin(angle));
    float corner_length        = (abs(angle_vector.x) < 0.1 || abs(angle_vector.y) < 0.1) ? 1.0 : sqrt(2.0);
    vec2  roundness_center     = start + radius * angle_vector * corner_length;
    vec2  outlineStart         = start + outlineThickness * angle_vector * corner_length;
    vec2  secondOutlineStart   = start + (outlineThickness + secondOutlineThickness) * angle_vector * corner_length;
    vec2  outerOutlineEnd      = start - outerOutlineThickness * angle_vector * corner_length;
    float distance_from_center = distance(coord0, roundness_center);
    bool  inOutlineZone =
            (hasPrimaryOutline() && outlineThickness >= radius && is_within(coord0, outlineStart, start)) ||
            (hasSecondOutline() && outlineThickness + secondOutlineThickness >= radius &&
             is_within(coord0, secondOutlineStart, start));

    if (hasOuterOutline()) {
        vec4 outerOutlineOverlay = mix(coord_shadowColor, outerOutlineColor, outerOutlineColor.a);
        if (distance_from_center > radius + outerOutlineThickness - 0.5) {
            // antialiasing for the outer outline to shadow
            float antialiasing = clamp(distance_from_center - radius - outerOutlineThickness + 0.5, 0.0, 1.0);
            return mix(outerOutlineOverlay, coord_shadowColor, antialiasing);
        } else if (!inOutlineZone && distance_from_center > radius - 0.5) {
            // antialiasing for the outer outline to the window edge
            float antialiasing = clamp(distance_from_center - radius + 0.5, 0.0, 1.0);
            if (hasPrimaryOutline()) {
                // if the primary outline is present
                vec4 outlineOverlay = vec4(mix(tex.rgb, outlineColor.rgb, outlineColor.a), 1.0);
                return mix(outlineOverlay, outerOutlineOverlay, antialiasing);
            } else if (hasSecondOutline()) {
                // if the second outline is present
                vec4 secondOutlineOverlay = vec4(mix(tex.rgb, secondOutlineColor.rgb, secondOutlineColor.a), 1.0);
                return mix(secondOutlineOverlay, outerOutlineOverlay, antialiasing);
            } else {
                // if the no other outline is not present
                return mix(tex, outerOutlineOverlay, antialiasing);
            }
        }
    } else {
        if (!inOutlineZone && distance_from_center > radius - 0.5) {
            // antialiasing for the outer outline to the window edge
            float antialiasing = clamp(distance_from_center - radius + 0.5, 0.0, 1.0);
            if (hasPrimaryOutline()) {
                // if the primary outline is present
                vec4 outlineOverlay = vec4(mix(tex.rgb, outlineColor.rgb, outlineColor.a), 1.0);
                return mix(outlineOverlay, coord_shadowColor, antialiasing);
            } else if (hasSecondOutline()) {
                // if the second outline is present
                vec4 secondOutlineOverlay = vec4(mix(tex.rgb, secondOutlineColor.rgb, secondOutlineColor.a), 1.0);
                return mix(secondOutlineOverlay, coord_shadowColor, antialiasing);
            } else {
                // if the no other outline is not present
                return mix(tex, coord_shadowColor, antialiasing);
            }
        }
    }

    if (hasPrimaryOutline()) {
        vec4 outlineOverlay = vec4(mix(tex.rgb, outlineColor.rgb, outlineColor.a), 1.0);

        if (outlineThickness >= radius && is_within(coord0, outlineStart, start)) {
            // when the outline is bigger than the roundness radius
            // from the window to the outline is sharp
            // no antialiasing is needed because it is not round
            return outlineOverlay;
        } else if (distance_from_center > radius - outlineThickness - 0.5) {
            // from the window to the outline
            float antialiasing = clamp(distance_from_center - radius + outlineThickness + 0.5, 0.0, 1.0);
            if (hasSecondOutline()) {
                vec4 secondOutlineOverlay = vec4(mix(tex.rgb, secondOutlineColor.rgb, secondOutlineColor.a), 1.0);
                return mix(secondOutlineOverlay, outlineOverlay, antialiasing);
            } else {
                return mix(tex, outlineOverlay, antialiasing);
            }
        }
    }

    if (hasSecondOutline()) {
        vec4 secondOutlineOverlay = vec4(mix(tex.rgb, secondOutlineColor.rgb, secondOutlineColor.a), 1.0);

        if (outlineThickness + secondOutlineThickness >= radius && is_within(coord0, secondOutlineStart, start)) {
            // when the outline is bigger than the roundness radius
            // from the window to the outline is sharp
            // no antialiasing is needed because it is not round
            return secondOutlineOverlay;
        } else if (distance_from_center > radius - outlineThickness - secondOutlineThickness - 0.5) {
            // from the window to the outline
            float antialiasing =
                    clamp(distance_from_center - radius + outlineThickness + secondOutlineThickness + 0.5, 0.0, 1.0);
            return mix(tex, secondOutlineOverlay, antialiasing);
        }
    }

    // if other conditions don't apply, just don't draw an outline, from the window to the shadow
    float antialiasing = clamp(radius - distance_from_center + 0.5, 0.0, 1.0);
    return mix(coord_shadowColor, tex, antialiasing);
}

vec4 run(vec2 texcoord0, vec4 tex)
{
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
            return shapeCorner(coord0, tex, vec2(0.0, 0.0), radians(45.0), coord_shadowColor); // Section TL
        } else if (coord0.x > windowSize.x - r) {
            return shapeCorner(coord0, tex, vec2(windowSize.x, 0.0), radians(135.0), coord_shadowColor); // Section TR
        } else if (coord0.y < outlineThickness + secondOutlineThickness) {
            return shapeCorner(coord0, tex, vec2(coord0.x, 0.0), radians(90.0), coord_shadowColor); // Section T
        }
    } else if (coord0.y > windowSize.y - r) {
        if (coord0.x < r) {
            return shapeCorner(coord0, tex, vec2(0.0, windowSize.y), radians(315.0), coord_shadowColor); // Section BL
        } else if (coord0.x > windowSize.x - r) {
            return shapeCorner(coord0, tex, vec2(windowSize.x, windowSize.y), radians(225.0),
                               coord_shadowColor); // Section BR
        } else if (coord0.y > windowSize.y - outlineThickness - secondOutlineThickness) {
            return shapeCorner(coord0, tex, vec2(coord0.x, windowSize.y), radians(270.0),
                               coord_shadowColor); // Section B
        }
    } else {
        if (coord0.x < r) {
            return shapeCorner(coord0, tex, vec2(0.0, coord0.y), radians(0.0), coord_shadowColor); // Section L
        } else if (coord0.x > windowSize.x - r) {
            return shapeCorner(coord0, tex, vec2(windowSize.x, coord0.y), radians(180.0),
                               coord_shadowColor); // Section R
        }
        // For section x, the tex is not changing
    }
    return tex;
}
