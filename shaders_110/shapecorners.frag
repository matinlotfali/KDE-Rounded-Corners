#version 110

uniform sampler2D front;
uniform float radius;
uniform bool windowActive;
uniform bool windowHasDecoration;
uniform vec2 windowSize;
uniform vec2 windowExpandedSize;
uniform vec2 windowTopLeft;
uniform vec4 shadowColor;
uniform vec4 outlineColor;
uniform float outlineThickness;

varying vec2 texcoord0;

bool isDrawingShadows() { return  windowHasDecoration && shadowColor.a > 0.0; }
bool isDrawingOutline() { return  outlineColor.a > 0.0 && outlineThickness > 0.0; }
bool isExtendedFrame(vec2 c) {
    return c.x < -1.0 || c.x > windowSize.x+1.0
        || c.y < -1.0 || c.y > windowSize.y+1.0;
}

vec4 shadowCorner(float distance_from_center, bool isTopCorner) {
    float r2 = radius * sqrt(2.0);
    float percent;
    if (windowActive) {
        if (isTopCorner)
            percent = (distance_from_center) / (r2 + 0.1*radius);
        else
            percent = (distance_from_center - 0.25*radius) / (r2 + 0.1*radius);
    }
    else {
        if (isTopCorner)
            percent = (distance_from_center + 0.55*radius) / (r2 + 0.5*radius);
        else
            percent = (distance_from_center) / (r2 + 0.1*radius);
    }
    return vec4(shadowColor.rgb, 1.0-percent);
}

vec4 shapeCorner(vec2 coord0, vec4 tex, vec2 center, bool isTopCorner) {
    float distance_from_center = distance(coord0, center);
    vec4 c = isDrawingShadows() ? shadowCorner(distance_from_center, backColor, isTopCorner) : backColor;

    if(isDrawingOutline()) {
        vec4 outlineOverlay = vec4(mix(tex.rgb, outlineColor.rgb, outlineColor.a), 1.0);

        if (distance_from_center < radius - outlineThickness/2.0) {
            float antialiasing = clamp(radius-outlineThickness+0.5-distance_from_center, 0.0, 1.0);
            return mix(outlineOverlay, tex, antialiasing);
        }
        else {
            float antialiasing = clamp(distance_from_center-radius+0.5, 0.0, 1.0);
            return mix(outlineOverlay, c, antialiasing);
        }
    }
    else {
        float antialiasing = clamp(radius-distance_from_center, 0.0, 1.0);
        return mix(c, tex, antialiasing);
    }
}

void main(void)
{
    vec4 tex = texture2D(sampler, texcoord0);
    vec2 coord0 = vec2(texcoord0.x * windowExpandedSize.x - windowTopLeft.x,
                  (1.0-texcoord0.y)* windowExpandedSize.y - windowTopLeft.y);

    if(!isExtendedFrame(coord0))
    {
        if (coord0.y < radius - 1.0) {
            if (coord0.x < radius - 1.0)
                tex = shapeCorner(coord0, tex, vec2(radius-1.0, radius-1.0), true);
            else if (coord0.x > windowSize.x - radius + 1.0)
                tex = shapeCorner(coord0, tex, vec2(windowSize.x - radius+1.0, radius-1.0), true);
            else if (coord0.y < outlineThickness - 1.0)
                tex.rgb = mix(tex.rgb, outlineColor.rgb, outlineColor.a);
        }
        else if (coord0.y > windowSize.y - radius) {
            if (coord0.x < radius - 1.0)
                tex = shapeCorner(coord0, tex, vec2(radius-1.0, windowSize.y - radius+1.0), false);
            else if (coord0.x > windowSize.x - radius + 1.0)
                tex = shapeCorner(coord0, tex, vec2(windowSize.x - radius+1.0, windowSize.y - radius+1.0), false);
            else if (coord0.y > windowSize.y - outlineThickness + 1.0)
                tex.rgb = mix(tex.rgb, outlineColor.rgb, outlineColor.a);
        }
        else {
            if (outlineColor.a > 0.0 && coord0.x < outlineThickness - 1.0)
                tex.rgb = mix(tex.rgb, outlineColor.rgb, outlineColor.a);
            else if (outlineColor.a > 0.0 && coord0.x > windowSize.x - outlineThickness + 1.0)
                tex.rgb = mix(tex.rgb, outlineColor.rgb, outlineColor.a);
        }
    }

    gl_FragColor = tex;
}
