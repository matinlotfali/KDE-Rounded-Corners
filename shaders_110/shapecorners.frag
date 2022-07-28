#version 110

uniform sampler2D sampler;
uniform sampler2D back;
uniform float radius;
uniform bool windowActive;
uniform vec2 windowSize;
uniform vec4 shadowColor;
uniform vec4 outlineColor;
uniform float outlineThickness;

uniform vec4 modulation;
uniform float saturation;

varying vec2 texcoord0;

vec4 goTowards(vec4 fromColor, vec4 toColor, float percent) {
    vec4 d = toColor - fromColor;
    return d * percent + fromColor;
}

vec4 shadowCorner(float distance_from_center, vec4 backColor, bool isTopCorner) {
    float r2 = radius * sqrt(2.0);
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

vec4 shapeCorner(vec2 coord0, vec4 tex, vec4 backColor, vec2 center, bool isTopCorner) {
    float distance_from_center = distance(coord0, center);
    if(distance_from_center < radius - outlineThickness)
        return tex;
    else if(outlineColor.a > 0.0 && distance_from_center < radius)
        return outlineColor;
    else {
        if(shadowColor.a > 0.0)
            return shadowCorner(distance_from_center, backColor, isTopCorner);
        else
            return backColor;
    }
}

bool isTitleBar(vec2 windowSize) {
    return shadowColor.a > 0.0 && windowSize.y < 40.0;
}

bool isExtendedFrame(vec4 color) {
    return shadowColor.a > 0.0 && color.a < 0.1;
}

void main(void)
{
    vec4 tex = texture2D(sampler, texcoord0);
    vec4 backColor = texture2D(back, texcoord0 * vec2(1.0,-1.0));
    vec2 backSize = windowSize;
    vec2 coord0 = vec2(texcoord0.x*windowSize.x, texcoord0.y*windowSize.y);


    if(isTitleBar(windowSize)) {
        coord0 += vec2(-1.0,-1.0);
        backColor = texture2D(back, vec2(coord0.x*backSize.x, (windowSize.y - coord0.y)*backSize.y));
    }

    if(!isExtendedFrame(tex))
    {

        if (coord0.x < radius) {
            if (coord0.y < radius)
            tex = shapeCorner(coord0, tex, backColor, vec2(radius, radius), false);
            else if (!isTitleBar(windowSize) && coord0.y > windowSize.y - radius)
            tex = shapeCorner(coord0, tex, backColor, vec2(radius, windowSize.y - radius), true);
            else if (outlineColor.a > 0.0 && coord0.x < outlineThickness)
            tex = outlineColor;
        }
        else if (coord0.x > windowSize.x - radius) {
            if (coord0.y < radius)
            tex = shapeCorner(coord0, tex, backColor, vec2(windowSize.x - radius, radius), false);
            else if (!isTitleBar(windowSize) && coord0.y > windowSize.y - radius)
            tex = shapeCorner(coord0, tex, backColor, vec2(windowSize.x - radius, windowSize.y - radius), true);
            else if (outlineColor.a > 0.0 && coord0.x > windowSize.x - outlineThickness)
            tex = outlineColor;
        }
        else if (outlineColor.a > 0.0) {
            if (coord0.y < outlineThickness)
            tex = outlineColor;
            else if (coord0.y > windowSize.y - outlineThickness)
            tex = outlineColor;
        }
    }

    if (saturation != 1.0) {
        vec3 desaturated = tex.rgb * vec3( 0.30, 0.59, 0.11 );
        desaturated = vec3( dot( desaturated, tex.rgb ));
        tex.rgb = tex.rgb * vec3( saturation ) + desaturated * vec3( 1.0 - saturation );
    }
    tex *= modulation;
    tex.rgb *= tex.a;

    gl_FragColor = tex;
}
