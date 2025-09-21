uniform sampler2D sampler;       // The painted contents of the window.
uniform float radius;            // The thickness of the outline in pixels specified in settings.
uniform vec2 windowSize;         // Containing `window->frameGeometry().size()`
uniform vec2 windowExpandedSize; // Containing `window->expandedGeometry().size()`

uniform vec2 windowTopLeft;      /* The distance between the top-left of `expandedGeometry` and
                                  * the top-left of `frameGeometry`. When `windowTopLeft = {0,0}`, it means
                                  * `expandedGeometry = frameGeometry` and there is no shadow. */

uniform vec4  outlineColor;       // The RGBA of the outline color specified in settings.
uniform float outlineThickness;   // The thickness of the outline in pixels specified in settings.
uniform vec4  secondOutlineColor; // The RGBA of the second outline color specified in settings.
uniform float secondOutlineThickness; // The thickness of the second outline in pixels specified in settings.
uniform vec4  outerOutlineColor;      // The RGBA of the outer outline color specified in settings.
uniform float outerOutlineThickness;  // The thickness of the outer outline in pixels specified in settings.

vec2 tex_to_pixel(vec2 texcoord) {
    return vec2(texcoord.x * windowExpandedSize.x - windowTopLeft.x,
                (1.0-texcoord.y)* windowExpandedSize.y - windowTopLeft.y);
}
vec2 pixel_to_tex(vec2 pixelcoord) {
    return vec2((pixelcoord.x + windowTopLeft.x) / windowExpandedSize.x,
                1.0-(pixelcoord.y + windowTopLeft.y) / windowExpandedSize.y);
}
bool hasExpandedSize() { return windowSize != windowExpandedSize; }
bool hasPrimaryOutline() { return outlineColor.a > 0.0 && outlineThickness > 0.0; }
bool hasSecondOutline() { return secondOutlineColor.a > 0.0 && secondOutlineThickness > 0.0; }
bool hasOuterOutline() { return hasExpandedSize() && outerOutlineColor.a > 0.0 && outerOutlineThickness > 0.0; }
