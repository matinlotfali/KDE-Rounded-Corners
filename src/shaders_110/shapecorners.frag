#version 110

uniform sampler2D front;
uniform float radius;
uniform bool disableRoundedTile;
uniform vec2 windowSize;
uniform vec2 windowExpandedSize;
uniform vec2 windowTopLeft;
uniform vec4 shadowColor;
uniform float shadowSize;
uniform vec4 outlineColor;
uniform float outlineThickness;
uniform vec4 modulation;
uniform float saturation;

varying vec2 texcoord0;

bool isDrawingShadows() { return  windowSize != windowExpandedSize && shadowColor.a > 0.0; }
bool isDrawingOutline() { return  outlineColor.a > 0.0 && outlineThickness > 0.0; }

vec4 shadowCorner(float distance_from_center) {
    float percent = -distance_from_center/shadowSize + 1.0;
    if (percent < 0.0)
        return vec4(0.0, 0.0, 0.0, 0.0);
    else
        return vec4(shadowColor.rgb * shadowColor.a * percent, shadowColor.a * percent);
}

vec4 shapeCorner(vec2 coord0, vec4 tex, vec2 start, float angle) {
    bool diagonal = abs(cos(angle)) > 0.1 && abs(sin(angle)) > 0.1;

    vec2 center;
    float distance_from_center;
    vec4 c;
    float r;

    if (disableRoundedTile) {
        r = outlineThickness;
        center = start + r * vec2(cos(angle), sin(angle));
        distance_from_center = distance(coord0, center);
        c = tex;
    }
    else {
        r = radius;
        center = start + radius * (diagonal? sqrt(2.0) : 1.0) * vec2(cos(angle), sin(angle));
        distance_from_center = distance(coord0, center);
        c = isDrawingShadows() ? shadowCorner(distance_from_center) : vec4(0.0, 0.0, 0.0, 0.0);
    }

    if(isDrawingOutline()) {
        vec4 outlineOverlay = vec4(mix(tex.rgb, outlineColor.rgb, outlineColor.a), 1.0);

        if (distance_from_center < r - outlineThickness+0.5) {
            float antialiasing = clamp(r-outlineThickness+0.5-distance_from_center, 0.0, 1.0);
            return mix(outlineOverlay, tex, antialiasing);
        }
        else {
            float antialiasing = clamp(distance_from_center-r+0.5, 0.0, 1.0);
            return mix(outlineOverlay, c, antialiasing);
        }
    }
    else {
        float antialiasing = clamp(r-distance_from_center, 0.0, 1.0);
        return mix(c, tex, antialiasing);
    }
}

void main(void)
{
    vec4 tex = texture2D(front, texcoord0);
    vec2 coord0 = vec2(texcoord0.x * windowExpandedSize.x - windowTopLeft.x,
                  (1.0-texcoord0.y)* windowExpandedSize.y - windowTopLeft.y);

    float r = disableRoundedTile? outlineThickness: radius;

    if (coord0.y < r) {
        if (coord0.x < r)
            tex = shapeCorner(coord0, tex, vec2(0.0, 0.0), radians(45.0));
        else if (coord0.x > windowSize.x - r)
            tex = shapeCorner(coord0, tex, vec2(windowSize.x, 0.0), radians(135.0));
        else if (coord0.y < outlineThickness)
            tex = shapeCorner(coord0, tex, vec2(coord0.x, 0.0), radians(90.0));
    }
    else if (coord0.y > windowSize.y - r) {
        if (coord0.x < r)
            tex = shapeCorner(coord0, tex, vec2(0.0, windowSize.y), radians(315.0));
        else if (coord0.x > windowSize.x - r)
            tex = shapeCorner(coord0, tex, vec2(windowSize.x, windowSize.y), radians(225.0));
        else if (coord0.y > windowSize.y - outlineThickness)
            tex = shapeCorner(coord0, tex, vec2(coord0.x, windowSize.y), radians(270.0));
    }
    else {
        if (coord0.x < radius)
            tex = shapeCorner(coord0, tex, vec2(0.0, coord0.y), radians(0.0));
        else if (coord0.x > windowSize.x - radius)
            tex = shapeCorner(coord0, tex, vec2(windowSize.x, coord0.y), radians(180.0));
    }

    if (saturation != 1.0) {
        vec3 desaturated = tex.rgb * vec3( 0.30, 0.59, 0.11 );
        desaturated = vec3( dot( desaturated, tex.rgb ));
        tex.rgb = tex.rgb * vec3( saturation ) + desaturated * vec3( 1.0 - saturation );
    }
    tex *= modulation;

    gl_FragColor = tex;
}
