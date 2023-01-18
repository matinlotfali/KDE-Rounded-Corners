#version 140

uniform sampler2D back;
uniform float radius;
uniform bool windowActive;
uniform bool windowHasDecoration;
uniform vec2 windowSize;
uniform vec4 shadowColor;
uniform vec4 outlineColor;
uniform float outlineThickness;
uniform vec4 modulation;
uniform float saturation;

in vec2 texcoord0;
out vec4 fragColor;

bool isDrawingShadows() { return  windowHasDecoration && shadowColor.a > 0.0; }
bool isDrawingOutline() { return  outlineColor.a > 0.0 && outlineThickness > 0.0; }

vec4 shadowCorner(float distance_from_center, vec4 backColor, bool isTopCorner) {
    float r2 = radius * sqrt(2.0);
    if (windowActive) {
        if (isTopCorner) {
            float percent = (distance_from_center - 0.25*radius) / (r2+0.1*radius);
            return mix(shadowColor, backColor, percent);
        }
        else {
            float percent = (distance_from_center + 0.15*radius) / (r2+0.2*radius);
            return mix(shadowColor, backColor, percent);
        }
    }
    else {
        if (isTopCorner) {
            float percent = (distance_from_center) / (r2+0.1*radius);
            return mix(shadowColor, backColor, percent);
        }
        else {
            float percent = (distance_from_center + 0.55*radius) / (r2+0.5*radius);
            return mix(shadowColor, backColor, percent);
        }
    }
}

vec4 shapeCorner(vec2 coord0, vec4 backColor, vec2 center, bool isTopCorner) {
    float distance_from_center = distance(coord0, center);
    vec4 c = isDrawingShadows() ? shadowCorner(distance_from_center, backColor, isTopCorner) : backColor;

    if(isDrawingOutline()) {
        if (distance_from_center < radius - outlineThickness/2.0) {
            float antialiasing = clamp(radius-outlineThickness+0.5-distance_from_center, 0.0, 1.0);
            return mix(outlineColor, vec4(0.0), antialiasing);
        }
        else {
            float antialiasing = clamp(distance_from_center-radius+0.5, 0.0, 1.0);
            return mix(outlineColor, c, antialiasing);
        }
    }
    else {
        float antialiasing = clamp(radius-distance_from_center, 0.0, 1.0);
        return mix(c, vec4(0.0), antialiasing);
    }
}

void main(void)
{
    vec4 tex = vec4(0.0);
    vec4 backColor = texture2D(back, texcoord0);
    vec2 coord0 = vec2(texcoord0.x*windowSize.x, texcoord0.y*windowSize.y);

    if (coord0.y < radius) {
        if (coord0.x < radius)
            tex = shapeCorner(coord0, backColor, vec2(radius, radius), true);
        else if (coord0.x > windowSize.x - radius)
            tex = shapeCorner(coord0, backColor, vec2(windowSize.x - radius, radius), true);
        else if (coord0.y < outlineThickness)
            tex = outlineColor;
    }
    else if (coord0.y > windowSize.y - radius) {
        if (coord0.x < radius)
            tex = shapeCorner(coord0, backColor, vec2(radius, windowSize.y - radius), false);
        else if (coord0.x > windowSize.x - radius)
            tex = shapeCorner(coord0, backColor, vec2(windowSize.x - radius, windowSize.y - radius), false);
        else if (coord0.y > windowSize.y - outlineThickness)
            tex = outlineColor;
    }
    else {        
        if (outlineColor.a > 0.0 && coord0.x < outlineThickness)
            tex = outlineColor;
        else if (outlineColor.a > 0.0 && coord0.x > windowSize.x - outlineThickness)
            tex = outlineColor;
    }

    fragColor = tex;
}
