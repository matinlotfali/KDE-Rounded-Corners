
uniform bool  useSquircleShape; // Whether to use a superellipse/squircle curve for corners.
uniform float squircleBlend;    // How strongly to blend circular corners toward a squircle.

const float squirclePower = 4.0;

/*
 *  \brief Length of a vector under the corner metric: a plain circle, or a circle/superellipse
 *         blend when squircles are enabled. Shared by the window edge (shape_distance) and the
 *         shadow (getShadowByDistance) so both round their corners with the same shape.
 *  \param point: The XY point being shaded.
 *  \param center: The XY center of the corner roundness.
 *  \return The blended length.
 */
float squircle_distance(vec2 point, vec2 center)
{
    vec2  delta    = point - center;
    vec2  d        = abs(delta);
    float circle   = length(d);
    float squircle = pow(pow(d.x, squirclePower) + pow(d.y, squirclePower), 1.0 / squirclePower);

    return mix(circle, squircle, squircleBlend);
}

/*
 *  \brief Magnitude of the gradient of squircle_length. The superellipse blend is not a true
 *         signed-distance field, so its gradient dips below 1 toward the diagonal; shape_distance
 *         divides by this so the outline and antialiasing bands stay a uniform width all around.
 *  \param point: The XY point being shaded.
 *  \param center: The XY center of the corner roundness.
 *  \return The gradient magnitude (1.0 for the plain-circle case).
 */
float squircle_gradient(vec2 point, vec2 center)
{
    vec2  delta              = point - center;
    vec2  d                  = abs(delta);
    float circle             = length(d);
    float squircle           = pow(pow(d.x, squirclePower) + pow(d.y, squirclePower), 1.0 / squirclePower);
    vec2  circle_gradient    = (circle > 0.0) ? d / circle : vec2(0.0);
    vec2  squircle_gradient_ = (squircle > 0.0) ? pow(d / squircle, vec2(squirclePower - 1.0)) : vec2(0.0);

    return length(mix(circle_gradient, squircle_gradient_, squircleBlend));
}
