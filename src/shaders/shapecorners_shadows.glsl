#include "variables.glsl"

uniform bool usesNativeShadows;
uniform vec4 shadowColor;        // The RGBA of the shadow color specified in settings.
uniform float shadowSize;        // The shadow size specified in settings.

bool isDrawingShadows() { return hasExpandedSize() && (usesNativeShadows || shadowColor.a > 0.0); }

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

vec4 getCustomShadow(vec2 coord0, float r) {
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

vec4 getNativeShadow(vec2 coord0, float r, vec4 default_tex) {
    float margin_edge = 2.0;
    float margin_point = margin_edge + 1.0;

    /*
        Split the window into these sections below. They will have a different center of circle for rounding.

        TL  T   T   TR
        L   x   x   R
        L   x   x   R
        BL  B   B   BR
    */
    if (coord0.y >= -margin_edge && coord0.y <= r) {
        if (coord0.x >= -margin_edge && coord0.x <= r) {
            vec2 a = vec2(-margin_point, coord0.y+coord0.x+margin_point);
            vec2 b = vec2(coord0.x+coord0.y+margin_point, -margin_point);
            vec4 a_color = texture2D(sampler, pixel_to_tex(a));
        vec4 b_color = texture2D(sampler, pixel_to_tex(b));
        return mix(a_color, b_color, distance(a, coord0)/distance(a,b));          // Section TL

        } else if (coord0.x <= windowSize.x + margin_edge && coord0.x >= windowSize.x - r) {
            vec2 a = vec2(windowSize.x+margin_point, coord0.y+(windowSize.x-coord0.x)+margin_point);
            vec2 b = vec2(coord0.x-coord0.y-margin_point, -margin_point);
            vec4 a_color = texture2D(sampler, pixel_to_tex(a));
        vec4 b_color = texture2D(sampler, pixel_to_tex(b));
        return mix(a_color, b_color, distance(a, coord0)/distance(a,b));          // Section TR

        }
    }
    else if (coord0.y <= windowSize.y + margin_edge && coord0.y >= windowSize.y - r) {
        if (coord0.x >= -margin_edge && coord0.x <= r) {
            vec2 a = vec2(-margin_point, coord0.y-coord0.x-margin_point);
            vec2 b = vec2(coord0.x+(windowSize.y-coord0.y)+margin_point, windowSize.y+margin_point);
            vec4 a_color = texture2D(sampler, pixel_to_tex(a));
        vec4 b_color = texture2D(sampler, pixel_to_tex(b));
        return mix(a_color, b_color, distance(a, coord0)/distance(a,b));          // Section BL

        } else if (coord0.x <= windowSize.x + margin_edge && coord0.x >= windowSize.x - r) {
            vec2 a = vec2(windowSize.x+margin_point, coord0.y-(windowSize.x-coord0.x)-margin_point);
            vec2 b = vec2(coord0.x-(windowSize.y-coord0.y)-margin_point, windowSize.y+margin_point);
            vec4 a_color = texture2D(sampler, pixel_to_tex(a));
        vec4 b_color = texture2D(sampler, pixel_to_tex(b));
        return mix(a_color, b_color, distance(a, coord0)/distance(a,b));          // Section BR
        }
    }
    return default_tex;
}

/*
 *  \brief This function is used to choose the pixel shadow color based on the XY pixel and corner radius.
 *  \param coord0: The XY point
 *  \param r: The radius of corners in pixel.
 *  \return The RGBA color to be used for the shadow.
 */
vec4 getShadow(vec2 coord0, float r, vec4 default_tex) {
    if(!isDrawingShadows()) {
        return vec4(0.0, 0.0, 0.0, 0.0);
    } else if (usesNativeShadows) {
        return getNativeShadow(coord0, r, default_tex);
    } else {
        return getCustomShadow(coord0, r);
    }
}
