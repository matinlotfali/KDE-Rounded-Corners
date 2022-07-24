#version 140

uniform sampler2D sampler;
uniform float radius;
uniform vec2 windowSize;
uniform bool windowActive;
uniform vec4 shadowColor;
uniform vec4 outlineColor;
uniform float outlineThickness;

in vec2 texcoord0;
out vec4 fragColor;

vec4 goTowards(vec4 fromColor, vec4 toColor, float percent) {
    vec4 d = toColor - fromColor;
    return d * percent + fromColor;
}

vec4 shadowCorner(float distance_from_center, vec4 backColor, bool isTopCorner) {
    float r2 = radius * sqrt(2);
    if (windowActive) {
        if (isTopCorner) {
            float percent = (distance_from_center + 0.15*radius) / (r2+0.2*radius);
            return goTowards(shadowColor, backColor, percent);
        }
        else {
            float percent = (distance_from_center - 0.25*radius) / (r2+0.1*radius);
            return goTowards(shadowColor, backColor, percent);
        }
    }
    else {
        if (isTopCorner) {
            float percent = (distance_from_center + 0.55*radius) / (r2+0.5*radius);
            return goTowards(shadowColor, backColor, percent);
        }
        else {
            float percent = (distance_from_center) / (r2+0.1*radius);
            return goTowards(shadowColor, backColor, percent);
        }
    }
}

vec4 shapeCorner(vec2 coord0, vec4 backColor, vec2 center, bool isTopCorner) {
    float distance_from_center = distance(coord0, center);
    if(distance_from_center < radius - outlineThickness)
        backColor.a = 0;
    else if(distance_from_center < radius)
        backColor = outlineColor;
    else {
        if(shadowColor.a > 0)
            backColor = shadowCorner(distance_from_center, backColor, isTopCorner);
        else
            backColor.a = 1;
    }
    return backColor;
}

void main(void)
{
    vec4 tex = texture(sampler, texcoord0);
    vec2 coord0 = vec2(texcoord0.x*windowSize.x, texcoord0.y*windowSize.y);
    if(coord0.x < radius) {
        if(coord0.y < radius)
            tex = shapeCorner(coord0, tex, vec2(radius, radius), false);
        else if (coord0.y > windowSize.y - radius)
            tex = shapeCorner(coord0, tex, vec2(radius, windowSize.y - radius), true);
        else if (coord0.x < outlineThickness)
            tex = outlineColor;
        else
            tex.a = 0;
    }
    else if(coord0.x > windowSize.x - radius) {
        if(coord0.y < radius)
            tex = shapeCorner(coord0, tex, vec2(windowSize.x - radius, radius), false);
        else if (coord0.y > windowSize.y - radius)
            tex = shapeCorner(coord0, tex, vec2(windowSize.x - radius, windowSize.y - radius), true);
        else if (coord0.x > windowSize.x - outlineThickness)
            tex = outlineColor;
        else
            tex.a = 0;
    }
    else {
        if (coord0.y < outlineThickness)
            tex = outlineColor;
        else if (coord0.y > windowSize.y - outlineThickness)
            tex = outlineColor;
        else
            tex.a = 0;
    }
    fragColor = tex;
}
