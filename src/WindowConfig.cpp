#include "WindowConfig.h"
#include <QPalette>
#include <QWidget>
#include "Config.h"

namespace
{
    /**
     * @brief Clamps a value to the [0, config] range depending on the direction of delta.
     * @param value The value to clamp.
     * @param delta The direction of change.
     * @param config The configuration limit.
     * @return The clamped value.
     */
    constexpr float clamp(const float value, const float delta, const float config)
    {
        if (delta > 0 && value > config) {
            return config;
        }
        if (delta < 0 && value < 0) {
            return 0;
        }
        return value;
    }

    /**
     * @brief Rounds a floating-point value to the specified number of decimal places.
     * @param value The value to round.
     * @return The rounded value.
     */
    constexpr float round_two_decimal(const float value)
    {
        constexpr float ROUNDING_FACTOR = 100.0F; // Factor for rounding to two decimal places
        return std::round(value * ROUNDING_FACTOR) / ROUNDING_FACTOR;
    }

    /**
     * @brief Resolves a color from settings: either a palette role or a custom color, with alpha applied.
     * @param palette The palette holding the current highlight colors.
     * @param group The palette color group (Active or Inactive).
     * @param usePalette Whether to use the palette role instead of the custom color.
     * @param paletteRole The palette color role index.
     * @param customColor The custom color to use when not using the palette.
     * @param alpha The alpha channel to apply [0, 255].
     * @return The resolved FloatColor.
     */
    ShapeCorners::FloatColor resolveColor(const QPalette &palette, const QPalette::ColorGroup group,
                                          const bool usePalette, const uint paletteRole, const QColor &customColor,
                                          const int alpha)
    {
        ShapeCorners::FloatColor color =
                usePalette
                        ? ShapeCorners::FloatColor(palette.color(group, static_cast<QPalette::ColorRole>(paletteRole)))
                        : ShapeCorners::FloatColor(customColor);
        color.setAlpha(alpha);
        return color;
    }

    /**
     * @brief Applies the second gradient stop and angle to an outline, or leaves it solid.
     * @param outline The outline to update. Its color1 must already be set.
     * @param isGradient Whether the outline uses a gradient.
     * @param color2 The custom second-stop color; its own alpha is used.
     * @param angleDeg The gradient angle in degrees.
     * @note When not a gradient, color2 mirrors color1 and the angle is zeroed, so the shader draws a solid color.
     */
    void applyGradient(ShapeCorners::OutlineConfig &outline, const bool isGradient, const QColor &color2,
                       const int angleDeg)
    {
        if (isGradient) {
            outline.color2 = ShapeCorners::FloatColor(color2);
            outline.angle  = static_cast<float>(angleDeg);
        } else {
            outline.color2 = outline.color1;
            outline.angle  = 0.0F;
        }
    }
} // namespace

namespace ShapeCorners
{
    /**
     * \brief Used only for its `palette()` function which holds the currently active highlight colors.
     */
    const static QWidget m_widget{};
} // namespace ShapeCorners

ShapeCorners::OutlineConfig ShapeCorners::OutlineConfig::operator+(const OutlineConfig &other) const
{
    return OutlineConfig{.size   = size + other.size,
                         .angle  = angle + other.angle,
                         .color1 = color1 + other.color1,
                         .color2 = color2 + other.color2};
}

ShapeCorners::OutlineConfig ShapeCorners::OutlineConfig::operator-(const OutlineConfig &other) const
{
    return OutlineConfig{.size   = size - other.size,
                         .angle  = angle - other.angle,
                         .color1 = color1 - other.color1,
                         .color2 = color2 - other.color2};
}

ShapeCorners::OutlineConfig ShapeCorners::OutlineConfig::operator*(const float scalar) const
{
    return OutlineConfig{
            .size = size * scalar, .angle = angle * scalar, .color1 = color1 * scalar, .color2 = color2 * scalar};
}

ShapeCorners::OutlineConfig ShapeCorners::OutlineConfig::operator/(const float scalar) const
{
    return OutlineConfig{
            .size = size / scalar, .angle = angle / scalar, .color1 = color1 / scalar, .color2 = color2 / scalar};
}

void ShapeCorners::OutlineConfig::operator+=(const OutlineConfig &other)
{
    size += other.size;
    angle += other.angle;
    color1 += other.color1;
    color2 += other.color2;
}

bool ShapeCorners::OutlineConfig::operator!() const { return size <= 0 && !color1 && !color2; }

void ShapeCorners::OutlineConfig::round()
{
    size  = ::round_two_decimal(size);
    angle = ::round_two_decimal(angle);
    color1.round();
    color2.round();
}

void ShapeCorners::OutlineConfig::clamp(const OutlineConfig &direction, const OutlineConfig &destination)
{
    size  = ::clamp(size, direction.size, destination.size);
    angle = ::clamp(angle, direction.angle, destination.angle);
    color1.clamp();
    color2.clamp();
}

ShapeCorners::WindowConfig ShapeCorners::WindowConfig::activeWindowConfig()
{
    const QPalette                &palette = m_widget.palette();
    constexpr QPalette::ColorGroup group   = QPalette::Active;

    WindowConfig config;
    config.cornerRadius = static_cast<float>(Config::size());
    config.shadowSize   = static_cast<float>(Config::shadowSize());
    config.shadowColor  = resolveColor(palette, group, Config::activeShadowUsePalette(), Config::activeShadowPalette(),
                                       Config::shadowColor(), Config::activeShadowAlpha());

    config.outline.size = static_cast<float>(Config::outlineThickness());
    config.outline.color1 =
            resolveColor(palette, group, Config::activeOutlineUsePalette(), Config::activeOutlinePalette(),
                         Config::outlineColor(), Config::activeOutlineAlpha());
    applyGradient(config.outline, Config::outlineIsGradient(), Config::outlineColor2(), Config::outlineGradientAngle());

    config.secondOutline.size = static_cast<float>(Config::secondOutlineThickness());
    config.secondOutline.color1 =
            resolveColor(palette, group, Config::activeSecondOutlineUsePalette(), Config::activeSecondOutlinePalette(),
                         Config::secondOutlineColor(), Config::activeSecondOutlineAlpha());
    applyGradient(config.secondOutline, Config::secondOutlineIsGradient(), Config::secondOutlineColor2(),
                  Config::secondOutlineGradientAngle());

    config.outerOutline.size = static_cast<float>(Config::outerOutlineThickness());
    config.outerOutline.color1 =
            resolveColor(palette, group, Config::activeOuterOutlineUsePalette(), Config::activeOuterOutlinePalette(),
                         Config::outerOutlineColor(), Config::activeOuterOutlineAlpha());
    applyGradient(config.outerOutline, Config::outerOutlineIsGradient(), Config::outerOutlineColor2(),
                  Config::outerOutlineGradientAngle());

    return config;
}

ShapeCorners::WindowConfig ShapeCorners::WindowConfig::inactiveWindowConfig()
{
    const QPalette                &palette = m_widget.palette();
    constexpr QPalette::ColorGroup group   = QPalette::Inactive;

    WindowConfig config;
    config.cornerRadius = static_cast<float>(Config::inactiveCornerRadius());
    config.shadowSize   = static_cast<float>(Config::inactiveShadowSize());
    config.shadowColor =
            resolveColor(palette, group, Config::inactiveShadowUsePalette(), Config::inactiveShadowPalette(),
                         Config::inactiveShadowColor(), Config::inactiveShadowAlpha());

    config.outline.size = static_cast<float>(Config::inactiveOutlineThickness());
    config.outline.color1 =
            resolveColor(palette, group, Config::inactiveOutlineUsePalette(), Config::inactiveOutlinePalette(),
                         Config::inactiveOutlineColor(), Config::inactiveOutlineAlpha());
    applyGradient(config.outline, Config::inactiveOutlineIsGradient(), Config::inactiveOutlineColor2(),
                  Config::inactiveOutlineGradientAngle());

    config.secondOutline.size   = static_cast<float>(Config::inactiveSecondOutlineThickness());
    config.secondOutline.color1 = resolveColor(
            palette, group, Config::inactiveSecondOutlineUsePalette(), Config::inactiveSecondOutlinePalette(),
            Config::inactiveSecondOutlineColor(), Config::inactiveSecondOutlineAlpha());
    applyGradient(config.secondOutline, Config::inactiveSecondOutlineIsGradient(),
                  Config::inactiveSecondOutlineColor2(), Config::inactiveSecondOutlineGradientAngle());

    config.outerOutline.size   = static_cast<float>(Config::inactiveOuterOutlineThickness());
    config.outerOutline.color1 = resolveColor(palette, group, Config::inactiveOuterOutlineUsePalette(),
                                              Config::inactiveOuterOutlinePalette(),
                                              Config::inactiveOuterOutlineColor(), Config::inactiveOuterOutlineAlpha());
    applyGradient(config.outerOutline, Config::inactiveOuterOutlineIsGradient(), Config::inactiveOuterOutlineColor2(),
                  Config::inactiveOuterOutlineGradientAngle());

    return config;
}

ShapeCorners::WindowConfig ShapeCorners::WindowConfig::operator+(const WindowConfig &other) const
{
    return WindowConfig{.cornerRadius  = cornerRadius + other.cornerRadius,
                        .shadowSize    = shadowSize + other.shadowSize,
                        .shadowColor   = shadowColor + other.shadowColor,
                        .outline       = outline + other.outline,
                        .secondOutline = secondOutline + other.secondOutline,
                        .outerOutline  = outerOutline + other.outerOutline};
}

ShapeCorners::WindowConfig ShapeCorners::WindowConfig::operator-(const WindowConfig &other) const
{
    return WindowConfig{.cornerRadius  = cornerRadius - other.cornerRadius,
                        .shadowSize    = shadowSize - other.shadowSize,
                        .shadowColor   = shadowColor - other.shadowColor,
                        .outline       = outline - other.outline,
                        .secondOutline = secondOutline - other.secondOutline,
                        .outerOutline  = outerOutline - other.outerOutline};
}

ShapeCorners::WindowConfig ShapeCorners::WindowConfig::operator*(const float scalar) const
{
    return WindowConfig{.cornerRadius  = cornerRadius * scalar,
                        .shadowSize    = shadowSize * scalar,
                        .shadowColor   = shadowColor * scalar,
                        .outline       = outline * scalar,
                        .secondOutline = secondOutline * scalar,
                        .outerOutline  = outerOutline * scalar};
}

ShapeCorners::WindowConfig ShapeCorners::WindowConfig::operator/(const float scalar) const
{
    return WindowConfig{.cornerRadius  = cornerRadius / scalar,
                        .shadowSize    = shadowSize / scalar,
                        .shadowColor   = shadowColor / scalar,
                        .outline       = outline / scalar,
                        .secondOutline = secondOutline / scalar,
                        .outerOutline  = outerOutline / scalar};
}

bool ShapeCorners::WindowConfig::operator!() const
{
    return cornerRadius <= 0 && shadowSize <= 0 && !shadowColor && !outline && !secondOutline && !outerOutline;
}

void ShapeCorners::WindowConfig::operator+=(const WindowConfig &other)
{
    cornerRadius += other.cornerRadius;
    shadowSize += other.shadowSize;
    shadowColor += other.shadowColor;
    outline += other.outline;
    secondOutline += other.secondOutline;
    outerOutline += other.outerOutline;
}

void ShapeCorners::WindowConfig::round()
{
    cornerRadius = ::round_two_decimal(cornerRadius);
    shadowSize   = ::round_two_decimal(shadowSize);
    shadowColor.round();
    outline.round();
    secondOutline.round();
    outerOutline.round();
}

void ShapeCorners::WindowConfig::clamp(const WindowConfig &direction, const WindowConfig &destination)
{
    cornerRadius = ::clamp(cornerRadius, direction.cornerRadius, destination.cornerRadius);
    shadowSize   = ::clamp(shadowSize, direction.shadowSize, destination.shadowSize);
    shadowColor.clamp();
    outline.clamp(direction.outline, destination.outline);
    secondOutline.clamp(direction.secondOutline, destination.secondOutline);
    outerOutline.clamp(direction.outerOutline, destination.outerOutline);
}
