#version 140

uniform sampler2D sampler;
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
bool isTitleBar(vec2 windowSize) { return windowSize.y < 40.0; }
bool isExtendedFrame(vec4 c) { return c.a < 0.1; }

vec4 shadowCorner(float distance_from_center, vec4 backColor, bool isTopCorner) {
    float r2 = radius * sqrt(2.0);
    if (windowActive) {
        if (isTopCorner) {
            float percent = (distance_from_center + 0.15*radius) / (r2+0.2*radius);
            return mix(shadowColor, backColor, percent);
        }
        else {
            float percent = (distance_from_center - 0.25*radius) / (r2+0.1*radius);
            return mix(shadowColor, backColor, percent);
        }
    }
    else {
        if (isTopCorner) {
            float percent = (distance_from_center + 0.55*radius) / (r2+0.5*radius);
            return mix(shadowColor, backColor, percent);
        }
        else {
            float percent = (distance_from_center) / (r2+0.1*radius);
            return mix(shadowColor, backColor, percent);
        }
    }
}

vec4 shapeCorner(vec2 coord0, vec4 tex, vec4 backColor, vec2 center, bool isTopCorner) {
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
    vec4 tex = texture(sampler, texcoord0);
    vec4 backColor = texture(back, texcoord0 * vec2(1,-1));
    vec2 windowSize = textureSize(sampler, 0);
    vec2 backSize = textureSize(back, 0);
    vec2 coord0 = vec2(texcoord0.x*windowSize.x, texcoord0.y*windowSize.y);

    if(isTitleBar(windowSize) && windowHasDecoration) {
        windowSize.x = backSize.x;
        coord0 += vec2(-1.0,-1.0);
        backColor = texelFetch(back, ivec2(coord0.x*backSize.x, (windowSize.y - coord0.y)*backSize.y), 0);
    }

    if(!isExtendedFrame(tex))
    {
        if (coord0.y < radius) {
            if (isTitleBar(windowSize) || !windowHasDecoration) {
                if (coord0.x < radius)
                    tex = shapeCorner(coord0, tex, backColor, vec2(radius, radius), true);
                else if (coord0.x > windowSize.x - radius)
                    tex = shapeCorner(coord0, tex, backColor, vec2(windowSize.x - radius, radius), true);
                else if (coord0.y < outlineThickness)
                    tex.rgb = mix(tex.rgb, outlineColor.rgb, outlineColor.a);
            }
            else {
                if (outlineColor.a > 0.0 && coord0.x < outlineThickness)
                   tex.rgb = mix(tex.rgb, outlineColor.rgb, outlineColor.a);
                else if (outlineColor.a > 0.0 && coord0.x > windowSize.x - outlineThickness)
                    tex.rgb = mix(tex.rgb, outlineColor.rgb, outlineColor.a);
            }
        }
        else if (coord0.y > windowSize.y - radius) {
            if (!isTitleBar(windowSize)) {
                if (coord0.x < radius)
                    tex = shapeCorner(coord0, tex, backColor, vec2(radius, windowSize.y - radius), false);
                else if (coord0.x > windowSize.x - radius)
                    tex = shapeCorner(coord0, tex, backColor, vec2(windowSize.x - radius, windowSize.y - radius), false);
                else if (coord0.y > windowSize.y - outlineThickness)
                    tex.rgb = mix(tex.rgb, outlineColor.rgb, outlineColor.a);
            }
            else {
                if (outlineColor.a > 0.0 && coord0.x < outlineThickness)
                    tex.rgb = mix(tex.rgb, outlineColor.rgb, outlineColor.a);
                else if (outlineColor.a > 0.0 && coord0.x > windowSize.x - outlineThickness)
                   tex.rgb = mix(tex.rgb, outlineColor.rgb, outlineColor.a);
            }
        }
        else {
            if (outlineColor.a > 0.0 && coord0.x < outlineThickness)
                tex.rgb = mix(tex.rgb, outlineColor.rgb, outlineColor.a);
            else if (outlineColor.a > 0.0 && coord0.x > windowSize.x - outlineThickness)
                tex.rgb = mix(tex.rgb, outlineColor.rgb, outlineColor.a);
        }
    }

    if (saturation != 1.0) {
        vec3 desaturated = tex.rgb * vec3( 0.30, 0.59, 0.11 );
        desaturated = vec3( dot( desaturated, tex.rgb ));
        tex.rgb = tex.rgb * vec3( saturation ) + desaturated * vec3( 1.0 - saturation );
    }
    tex *= modulation;
    tex.rgb *= tex.a;

    fragColor = tex;
}
