#include "squircles.glsl"
#include "variables.glsl"

uniform bool  usesNativeShadows;
uniform vec4  shadowColor; // The RGBA of the shadow color specified in settings.
uniform float shadowSize;  // The shadow size specified in settings.

bool isDrawingShadows() { return hasExpandedSize() && (usesNativeShadows || shadowColor.a > 0.0); }

float parametricBlend(float t)
{
    float sqt = t * t;
    return sqt / (2.0 * (sqt - t) + 1.0);
}

/*
 *  \brief This function generates the shadow color based on the distance_from_center
 *  \param coord0: The XY point
 *  \param center: The origin XY point that is being used as a reference for the center of shadow darkness.
 *  \return The RGBA color to be used for the shadow.
 */
vec4 getShadowByDistance(vec2 coord0, vec2 center)
{
    float distance_from_center;
    if (useSquircleShape) {
        distance_from_center = squircle_distance(coord0, center);
    } else {
        distance_from_center = distance(coord0, center);
    }

    float percent = 1.0 - distance_from_center / shadowSize;
    percent       = clamp(percent, 0.0, 1.0);
    percent       = parametricBlend(percent);
    if (percent < 0.0) {
        return vec4(0.0, 0.0, 0.0, 0.0);
    }
    return vec4(shadowColor.rgb * shadowColor.a * percent, shadowColor.a * percent);
}

vec4 getCustomShadow(vec2 coord0, float r)
{
    float shadowShiftX   = sqrt(shadowSize);
    float shadowShiftTop = sqrt(shadowSize);

    vec2 spine_min = vec2(r + shadowShiftX, r + shadowShiftTop);
    vec2 spine_max = vec2(windowSize.x - r - shadowShiftX, windowSize.y - r);
    
    vec2 shadow_center = clamp(coord0, spine_min, spine_max);
    
    return getShadowByDistance(coord0, shadow_center);
}

vec4 getNativeShadow(vec2 coord0, float r, vec4 default_tex)
{
    // If we are on the straight edges, we MUST NOT reconstruct the shadow.
    // The native shadow on the straight edges is already perfect and must be preserved.
    if ((coord0.x >= r && coord0.x <= windowSize.x - r) || (coord0.y >= r && coord0.y <= windowSize.y - r)) {
        return default_tex;
    }

    // 1. Calculate exact continuous Euclidean distance field 'd'
    vec2 p = coord0 - windowSize * 0.5;
    vec2 b = windowSize * 0.5;
    vec2 q = abs(p) - b + vec2(r);
    vec2 q_pos = max(q, 0.0);
    
    float d_corner = 0.0;
    if (q_pos.x > 0.0 && q_pos.y > 0.0 && useSquircleShape) {
        float sq = squircle_distance(q_pos, vec2(0.0));
        float grad = max(length(squircle_gradient(q_pos, vec2(0.0))), 1e-4);
        d_corner = sq / grad;
    } else {
        d_corner = length(q_pos);
    }
    float d = min(max(q.x, q.y), 0.0) + d_corner - r;
    
    // If inside the window, just use the application's texture
    if (d <= 0.0) {
        return default_tex;
    }
    
    // 2. Reconstruct shadow for ALL apps
    // Clamp the sampling coordinates exactly to the start of the straight edges (r).
    // This perfectly avoids the corner black square, while seamlessly blending into the straight edge.
    float x_clamp = (coord0.x < windowSize.x * 0.5) ? max(coord0.x, r) : min(coord0.x, windowSize.x - r);
    float y_clamp = (coord0.y < windowSize.y * 0.5) ? max(coord0.y, r) : min(coord0.y, windowSize.y - r);
    
    float sample_y = (coord0.y < windowSize.y * 0.5) ? -d : windowSize.y + d;
    float sample_x = (coord0.x < windowSize.x * 0.5) ? -d : windowSize.x + d;
    
    vec2 p_horiz = vec2(x_clamp, sample_y);
    vec2 p_vert = vec2(sample_x, y_clamp);
    
    vec4 color_horiz = texture2D(sampler, pixel_to_tex(p_horiz));
    vec4 color_vert = texture2D(sampler, pixel_to_tex(p_vert));
    
    // 3. Radially mix based on nearest corner center
    float center_x = (coord0.x < windowSize.x * 0.5) ? r : windowSize.x - r;
    float center_y = (coord0.y < windowSize.y * 0.5) ? r : windowSize.y - r;
    vec2 center = vec2(center_x, center_y);
    
    vec2 delta = abs(coord0 - center);
    float angle_weight = delta.x / (delta.x + delta.y + 1e-4);
    
    // The mixture naturally becomes 100% color_horiz at the top edge and 100% color_vert at the left edge.
    return mix(color_horiz, color_vert, angle_weight);
}

/*
 *  \brief This function is used to choose the pixel shadow color based on the XY pixel and corner radius.
 *  \param coord0: The XY point
 *  \param r: The radius of corners in pixel.
 *  \return The RGBA color to be used for the shadow.
 */
vec4 getShadow(vec2 coord0, float r, vec4 default_tex)
{
    if (!isDrawingShadows()) {
        if (hasExpandedSize()) {
            // Preserve RGB so anti-aliasing only fades alpha (fix for #430)
            return vec4(default_tex.rgb, 0.0);
        }
        // Windows without a shadow buffer (e.g. CSD XWayland apps like Steam)
        // need premultiplied-valid output or the compositor brightens edge pixels (#491)
        return vec4(0.0, 0.0, 0.0, 0.0);
    } else if (usesNativeShadows) {
        return getNativeShadow(coord0, r, default_tex);
    } else {
        return getCustomShadow(coord0, r);
    }
}
