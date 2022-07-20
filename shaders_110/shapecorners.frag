#version 110

uniform sampler2D sampler;
uniform int radius;
uniform int cornerIndex;
uniform bool windowActive;
uniform vec4 shadowColor;
uniform vec4 outlineColor;

varying vec2 texcoord0;

vec4 goTowards(vec4 fromColor, vec4 toColor, float percent) {
    vec4 d = toColor - fromColor;
    return d * percent + fromColor;
}

vec4 shadowCorner(float distance_from_center, vec4 backColor, bool isTopCorner) {
    if (windowActive) {
        if (isTopCorner) {
            float percent = (distance_from_center + 0.15) / (sqrt(2.0)+0.2);
            return goTowards(shadowColor, backColor, percent);
        }
        else {
            float percent = (distance_from_center - 0.25) / (sqrt(2.0)+0.1);
            return goTowards(shadowColor, backColor, percent);
        }
    }
    else {
        if (isTopCorner) {
            float percent = (distance_from_center + 0.55) / (sqrt(2.0)+0.5);
            return goTowards(shadowColor, backColor, percent);
        }
        else {
            float percent = (distance_from_center) / (sqrt(2.0)+0.1);
            return goTowards(shadowColor, backColor, percent);
        }
    }
}

vec4 shapeCorner(vec2 texcoord0, vec4 backColor, vec2 center, bool isTopCorner) {
    float distance_from_center = distance(texcoord0, center);
    if(distance_from_center < 1 - 1.0f/radius)
        backColor.a = 0;
    else if(distance_from_center < 1)
        backColor = outlineColor;
    else {
        if(shadowColor.a > 0)
            backColor = shadowCorner(distance_from_center, backColor, isTopCorner);
        else
            backColor.a = 1;
    }
    return backColor;
}

void main()
{
    vec4 tex = texture(sampler, texcoord0);
    if(cornerIndex == 0)
        tex = shapeCorner(texcoord0, tex, vec2(1, 0), true);
    else if(cornerIndex == 1)
        tex = shapeCorner(texcoord0, tex, vec2(0, 0), true);
    else if(cornerIndex == 2)
        tex = shapeCorner(texcoord0, tex, vec2(0, 1), false);
    else if(cornerIndex == 3)
        tex = shapeCorner(texcoord0, tex, vec2(1, 1), false);
    fragColor = tex;
}
