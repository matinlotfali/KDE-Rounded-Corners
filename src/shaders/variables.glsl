uniform sampler2D sampler;            // The painted contents of the window.
uniform float     radius;             // The thickness of the outline in pixels specified in settings.
uniform vec2      windowSize;         // Containing `window->frameGeometry().size()`
uniform vec2      windowExpandedSize; // Containing `window->expandedGeometry().size()`

uniform vec2 windowTopLeft; /* The distance between the top-left of `expandedGeometry` and
                             * the top-left of `frameGeometry`. When `windowTopLeft = {0,0}`, it means
                             * `expandedGeometry = frameGeometry` and there is no shadow. */

uniform vec4  outlineColor1;          // The RGBA of the outline's first gradient stop specified in settings.
uniform vec4  outlineColor2;          // The RGBA of the outline's second gradient stop. Equals stop 1 when not a gradient.
uniform float outlineAngle;           // The outline gradient angle in degrees. 0 = left->right, 90 = top->bottom.
uniform float outlineThickness;       // The thickness of the outline in pixels specified in settings.
uniform vec4  secondOutlineColor1;    // The RGBA of the second outline's first gradient stop specified in settings.
uniform vec4  secondOutlineColor2;    // The RGBA of the second outline's second gradient stop.
uniform float secondOutlineAngle;     // The second outline gradient angle in degrees.
uniform float secondOutlineThickness; // The thickness of the second outline in pixels specified in settings.
uniform vec4  outerOutlineColor1;     // The RGBA of the outer outline's first gradient stop specified in settings.
uniform vec4  outerOutlineColor2;     // The RGBA of the outer outline's second gradient stop.
uniform float outerOutlineAngle;      // The outer outline gradient angle in degrees.
uniform float outerOutlineThickness;  // The thickness of the outer outline in pixels specified in settings.

#ifdef HAS_POSITION0
bool yInverted = true; /* Whether `texcoord0.y` grows downward from the top edge of the offscreen
                        * texture rather than upward from its bottom edge. Set by `main()`. */

/*
 *  \brief Measures which way `texcoord0.y` runs by comparing it with `position0.y`, which always grows downward.
 *  \note  This uses derivatives, so `main()` has to call it before any branching.
 *  \return Whether `texcoord0.y` grows downward.
 */
bool detect_y_inverted()
{
    return dFdx(texcoord0.y) * dFdx(position0.y) + dFdy(texcoord0.y) * dFdy(position0.y) > 0.0;
}
#else
const bool yInverted = false; // Without `position0`, KWin hands over a plain texture whose `texcoord0.y` grows upward.
#endif

vec2 tex_to_pixel(vec2 texcoord)
{
    float y = yInverted ? texcoord.y : 1.0 - texcoord.y;
    return vec2(texcoord.x * windowExpandedSize.x - windowTopLeft.x, y * windowExpandedSize.y - windowTopLeft.y);
}
vec2 pixel_to_tex(vec2 pixelcoord)
{
    float y = (pixelcoord.y + windowTopLeft.y) / windowExpandedSize.y;
    return vec2((pixelcoord.x + windowTopLeft.x) / windowExpandedSize.x, yInverted ? y : 1.0 - y);
}
bool hasExpandedSize() { return windowTopLeft.x >= 1.0 && windowTopLeft.y >= 1.0; }
bool hasPrimaryOutline() { return outlineColor1.a > 0.0 && outlineThickness > 0.0; }
bool hasSecondOutline() { return secondOutlineColor1.a > 0.0 && secondOutlineThickness > 0.0; }
bool hasOuterOutline() { return hasExpandedSize() && outerOutlineColor1.a > 0.0 && outerOutlineThickness > 0.0; }
