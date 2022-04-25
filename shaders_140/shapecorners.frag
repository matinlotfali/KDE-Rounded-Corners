#version 140

uniform sampler2D sampler;
uniform float radius;
uniform vec2 windowSize;

in vec2 texcoord0;
out vec4 fragColor;

void main(void)
{
    vec4 tex = texture(sampler, texcoord0);
    tex.a = 0;

    vec2 coord_px = texcoord0 * windowSize;

    if (coord_px.x < radius && coord_px.y > windowSize.y-radius)
    {
        vec2 center = vec2(radius,windowSize.y-radius);
        float distance_from_center = distance(coord_px, center);
        if(distance_from_center > radius) {
            tex.a = 1;
        }
    }
    else if (coord_px.x < radius && coord_px.y < radius)
    {
        vec2 center = vec2(radius, radius);
        float distance_from_center = distance(coord_px, center);
        if(distance_from_center > radius) {
            tex.a = 1;
        }
    }
    else if (coord_px.x > windowSize.x-radius && coord_px.y < radius)
    {
        vec2 center = vec2(windowSize.x-radius, radius);
        float distance_from_center = distance(coord_px, center);
        if(distance_from_center > radius) {
            tex.a = 1;
        }
    }
    else if (coord_px.x > windowSize.x-radius && coord_px.y > windowSize.y-radius)
    {
        vec2 center = vec2(windowSize.x-radius, windowSize.y-radius);
        float distance_from_center = distance(coord_px, center);
        if(distance_from_center > radius) {
            tex.a = 1;
        }
    }
    fragColor = tex;
}
