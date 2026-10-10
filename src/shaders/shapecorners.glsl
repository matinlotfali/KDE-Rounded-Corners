#include "shapecorners_shadows.glsl"

float window_sdf(vec2 coord, vec2 size, float r) {
    vec2 p = coord - size * 0.5;
    vec2 b = size * 0.5;
    vec2 q = abs(p) - b + vec2(r);
    vec2 q_pos = max(q, 0.0);
    
    float d_corner = 0.0;
    if (q_pos.x > 0.0 && q_pos.y > 0.0 && useSquircleShape) {
        float sq_d = squircle_distance(q_pos, vec2(0.0));
        float grad = squircle_gradient(q_pos, vec2(0.0));
        d_corner = (sq_d - r) / max(grad, 1e-4) + r;
    } else {
        d_corner = length(q_pos);
    }
    
    return min(max(q.x, q.y), 0.0) + d_corner - r;
}

vec4 getGradient(vec4 c1, vec4 c2, float angleDeg, vec2 coord0) {
    if (c1 == c2) return c1;
    vec2 dir = vec2(cos(radians(angleDeg)), sin(radians(angleDeg)));
    float t = clamp(dot(coord0 / windowSize - 0.5, dir) + 0.5, 0.0, 1.0);
    return mix(c1, c2, t);
}

vec4 getAutoOutlineColor(vec2 coord0, float r) {
    float x = coord0.x;
    float y = coord0.y;
    
    float safe_r = max(r, 2.0);
    float sample_x = clamp(x, safe_r, max(windowSize.x - safe_r, safe_r));
    float sample_y = clamp(y, safe_r, max(windowSize.y - safe_r, safe_r));
    
    float dist_x = min(x, windowSize.x - x);
    float dist_y = min(y, windowSize.y - y);
    
    vec2 edge_coord;
    if (dist_x < dist_y) {
        float edge_x = (x < windowSize.x * 0.5) ? 0.5 : windowSize.x - 0.5;
        edge_coord = vec2(edge_x, sample_y);
    } else {
        float edge_y = (y < windowSize.y * 0.5) ? 0.5 : windowSize.y - 0.5;
        edge_coord = vec2(sample_x, edge_y);
    }
    
    return texture2D(sampler, pixel_to_tex(edge_coord));
}

vec4 run(vec2 texcoord0, vec4 tex) {
    // We cannot return early for tex.a == 0.0 because CSD apps (like RustRover)
    // have transparent corners, and we still need to draw KWin's outline there!

    vec2 coord0 = tex_to_pixel(texcoord0);
    float r = max(radius, outlineThickness);
    
    // Quick exit for pixels deep inside the window to save GPU cycles
    vec2 dist_edges = min(coord0, windowSize - coord0);
    float max_dist = r + outlineThickness + secondOutlineThickness + outerOutlineThickness + 1.0;
    if (dist_edges.x > max_dist && dist_edges.y > max_dist) {
        return tex;
    }

    vec4 coord_shadowColor = getShadow(coord0, r, tex);
    float d = window_sdf(coord0, windowSize, radius);

    vec4 color = tex;

    if (hasSecondOutline()) {
        vec4 grad2 = getGradient(secondOutlineColor1, secondOutlineColor2, secondOutlineAngle, coord0);
        vec4 pre_grad2 = vec4(grad2.rgb * grad2.a, grad2.a);
        vec4 outColor2 = pre_grad2 + tex * (1.0 - pre_grad2.a);
        float t_in2 = clamp(d + outlineThickness + secondOutlineThickness + 0.5, 0.0, 1.0);
        color = mix(color, outColor2, t_in2);
    }

    if (hasPrimaryOutline()) {
        vec4 autoColor = getAutoOutlineColor(coord0, r);
        vec4 outColor1;
        
        if (autoColor.a > 0.1) {
            // Use the application's actual border color (forced opaque to avoid transparency holes)
            outColor1 = vec4(autoColor.rgb / autoColor.a, 1.0);
        } else {
            // Fallback to configured settings
            vec4 grad1 = getGradient(outlineColor1, outlineColor2, outlineAngle, coord0);
            vec4 pre_grad1 = vec4(grad1.rgb * grad1.a, grad1.a);
            outColor1 = pre_grad1 + tex * (1.0 - pre_grad1.a);
        }
        
        float t_in1 = clamp(d + outlineThickness + 0.5, 0.0, 1.0);
        color = mix(color, outColor1, t_in1);
    }
    
    if (hasOuterOutline()) {
        vec4 gradOuter = getGradient(outerOutlineColor1, outerOutlineColor2, outerOutlineAngle, coord0);
        vec4 outColorOuter = mix(coord_shadowColor, gradOuter, gradOuter.a);
        
        float t_edge = clamp(d + 0.5, 0.0, 1.0);
        color = mix(color, outColorOuter, t_edge);
        
        float t_out = clamp(d - outerOutlineThickness + 0.5, 0.0, 1.0);
        color = mix(color, coord_shadowColor, t_out);
    } else {
        float t_edge = clamp(d + 0.5, 0.0, 1.0);
        color = mix(color, coord_shadowColor, t_edge);
    }

    return color;
}
