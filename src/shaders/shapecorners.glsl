uniform float radius;            // The thickness of the outline in pixels specified in settings.
uniform vec2 windowSize;         // Containing `window->frameGeometry().size()`
uniform vec2 windowExpandedSize; // Containing `window->expandedGeometry().size()`
uniform bool hasRoundCorners;

uniform vec2 windowTopLeft;      /* The distance between the top-left of `expandedGeometry` and
                                  * the top-left of `frameGeometry`. When `windowTopLeft = {0,0}`, it means
                                  * `expandedGeometry = frameGeometry` and there is no shadow. */

uniform vec4 shadowColor;        // The RGBA of the shadow color specified in settings.
uniform float shadowSize;        // The shadow size specified in settings.
uniform vec4 outlineColor;       // The RGBA of the outline color specified in settings.
uniform float outlineThickness;  // The thickness of the outline in pixels specified in settings.
uniform vec4 secondOutlineColor; // The RGBA of the second outline color specified in settings.
uniform float secondOutlineThickness;  // The thickness of the second outline in pixels specified in settings.

vec2 tex_to_pixel(vec2 texcoord) {
    return vec2(texcoord0.x * windowExpandedSize.x - windowTopLeft.x,
                (1.0-texcoord0.y)* windowExpandedSize.y - windowTopLeft.y);
}
vec2 pixel_to_tex(vec2 pixelcoord) {
    return vec2((pixelcoord.x + windowTopLeft.x) / windowExpandedSize.x,
                1.0-(pixelcoord.y + windowTopLeft.y) / windowExpandedSize.y);
}
bool hasExpandedSize() { return windowSize != windowExpandedSize; }
bool isDrawingShadows() { return hasExpandedSize() && shadowColor.a > 0.0; }
bool hasPrimaryOutline() { return outlineColor.a > 0.0 && outlineThickness > 0.0; }
bool hasSecondOutline() { return hasExpandedSize() && secondOutlineColor.a > 0.0 && secondOutlineThickness > 0.0; }

float parametricBlend(float t) {
    float sqt = t * t;
    return sqt / (2.0 * (sqt - t) + 1.0);
}

/*
 *  \brief This function generates the shadow color based on the distance_from_center
 *  \param coord0: The XY point
 *  \param center: The origin XY point that is being used as a reference for the center of shadow darkness.
 *  \return The RGBA color to be used for the shadow.
 */
vec4 getShadowByDistance(vec2 coord0, vec2 center) {
    float distance_from_center = distance(coord0, center);
    float percent = 1.0 - distance_from_center/shadowSize;
    percent = clamp(percent, 0.0, 1.0);
    percent = parametricBlend(percent);
    if (percent < 0.0) {
        return vec4(0.0, 0.0, 0.0, 0.0);
    }
    return vec4(shadowColor.rgb * shadowColor.a * percent, shadowColor.a * percent);
}

/*
 *  \brief This function is used to choose the pixel shadow color based on the XY pixel and corner radius.
 *  \param coord0: The XY point
 *  \param r: The radius of corners in pixel.
 *  \return The RGBA color to be used for the shadow.
 */
vec4 getShadow(vec2 coord0, float r) {
    if(!isDrawingShadows()) {
        return vec4(0.0, 0.0, 0.0, 0.0);
    }
    float shadowShiftX = sqrt(shadowSize);
    float shadowShiftTop = sqrt(shadowSize);

    /*
        Split the window into these sections below. They will have a different center of circle for rounding.

        TL  T   T   TR
        L   x   x   R
        L   x   x   R
        BL  B   B   BR
    */
    if (coord0.y < r + shadowShiftTop) {
        if (coord0.x < r + shadowShiftX) {
            return getShadowByDistance(coord0, vec2(r+shadowShiftX, r+shadowShiftTop));               // Section TL
        } else if (coord0.x > windowSize.x - r - shadowShiftX) {
            return getShadowByDistance(coord0, vec2(windowSize.x -r-shadowShiftX, r+shadowShiftTop)); // Section TR
        } else if (coord0.y < 0.0) {
            return getShadowByDistance(coord0, vec2(coord0.x, r+shadowShiftTop));                     // Section T
        }
    }
    else if (coord0.y > windowSize.y - r) {
        if (coord0.x < r + shadowShiftX) {
            return getShadowByDistance(coord0, vec2(r+shadowShiftX, windowSize.y-r));                   // Section BL
        } else if (coord0.x > windowSize.x - r - shadowShiftX) {
            return getShadowByDistance(coord0, vec2(windowSize.x -r-shadowShiftX, windowSize.y - r));   // Section BR
        } else if (coord0.y > windowSize.y) {
            return getShadowByDistance(coord0, vec2(coord0.x, windowSize.y - r));                       // Section B
        }
    }
    else {
        if (coord0.x < 0.0) {
            return getShadowByDistance(coord0, vec2(r+shadowShiftX, coord0.y));                   // Section L
        } else if (coord0.x > windowSize.x) {
            return getShadowByDistance(coord0, vec2(windowSize.x -r-shadowShiftX, coord0.y));     // Section R
        }
        // For section x, the tex is not changing
    }
    return vec4(0.0, 0.0, 0.0, 0.0);
}

/*
 *  \brief This function is used to choose the pixel color based on its distance to the center input.
 *  \param coord0: The XY point
 *  \param tex: The RGBA color of the pixel in XY
 *  \param center: The origin XY point that is being used as a reference for rounding corners.
 *  \param shadowColor: The RGBA color of the shadow of the pixel behind the window
 *  \return The RGBA color to be used instead of tex input.
 */
vec4 shapeCorner(vec2 coord0, vec4 tex, vec2 start, float angle, vec4 coord_shadowColor) {
    float diagonal_length = (hasRoundCorners && abs(cos(angle)) > 0.1 && abs(sin(angle)) > 0.1) ? sqrt(2.0) : 1.0;
    float r = hasRoundCorners ? radius: outlineThickness;
    vec2 center = start + r * diagonal_length * vec2(cos(angle), sin(angle));
    float distance_from_center = distance(coord0, center);

    vec4 secondaryOutlineOverlay = mix(coord_shadowColor, secondOutlineColor, secondOutlineColor.a);
    if (tex.a > 0.1 && hasPrimaryOutline()) {
        vec4 outlineOverlay = vec4(mix(tex.rgb, outlineColor.rgb, outlineColor.a), 1.0);

        if (distance_from_center < r - outlineThickness + 0.5) {
            // from the window to the outline
            float antialiasing = clamp(r - outlineThickness + 0.5 - distance_from_center, 0.0, 1.0);
            return mix(outlineOverlay, tex, antialiasing);
        }
        else if (hasSecondOutline()) {

            if (distance_from_center < r + 0.5) {
                // from the second outline to the shadow
                float antialiasing = clamp(r + 0.5 - distance_from_center, 0.0, 1.0);
                return mix(secondaryOutlineOverlay, outlineOverlay, antialiasing);
            }
            else {
                // from the second outline to the shadow
                float antialiasing = clamp(distance_from_center - r - secondOutlineThickness + 0.5, 0.0, 1.0);
                return mix(secondaryOutlineOverlay, coord_shadowColor, antialiasing);
            }
        } else {
            // from the first outline to the shadow
            float antialiasing = clamp(distance_from_center - r + 0.5, 0.0, 1.0);
            return mix(outlineOverlay, coord_shadowColor, antialiasing);
        }
    }
    else if (hasSecondOutline()) {
        if (distance_from_center < r + 0.5) {
            // from window to the second outline
            float antialiasing = clamp(r + 0.5 - distance_from_center, 0.0, 1.0);
            return mix(secondaryOutlineOverlay, tex, antialiasing);
        }
        else {
            // from the second outline to the shadow
            float antialiasing = clamp(distance_from_center - r - secondOutlineThickness + 0.5, 0.0, 1.0);
            return mix(secondaryOutlineOverlay, coord_shadowColor, antialiasing);
        }
    }

    // if other conditions don't apply, just don't draw an outline, from the window to the shadow
    float antialiasing = clamp(r - distance_from_center + 0.5, 0.0, 1.0);
    return mix(coord_shadowColor, tex, antialiasing);
}

vec4 run(vec4 tex) {
    if(tex.a == 0.0) {
        return tex;
    }

    float r = hasRoundCorners? radius: outlineThickness;

    /* Since `texcoord0` is ranging from {0.0, 0.0} to {1.0, 1.0} is not pixel intuitive,
     * I am changing the range to become from {0.0, 0.0} to {width, height}
     * in a way that {0,0} is the top-left of the window and not its shadow.
     * This means areas with negative numbers and areas beyond windowSize is considered part of the shadow. */
    vec2 coord0 = tex_to_pixel(texcoord0);

    vec4 coord_shadowColor = getShadow(coord0, r);

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

