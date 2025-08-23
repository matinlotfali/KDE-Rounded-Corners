#include "WindowConfig.h"
#include <QWidget>
#include "Config.h"

namespace ShapeCorners
{
    /**
     * \brief Used only for its `palette()` function which holds the currently active highlight colors.
     */
    const static QWidget m_widget{};

    /**
     * @brief Clamps a value to the [0, config] range depending on the direction of delta.
     * @param value The value to clamp.
     * @param delta The direction of change.
     * @param config The configuration limit.
     * @return The clamped value.
     */
    constexpr static float clamp(const float value, const float delta, const float config)
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
    constexpr static float round(const float value)
    {
        constexpr float ROUNDING_FACTOR = 100.0F; // Factor for rounding to two decimal places
        return std::round(value * ROUNDING_FACTOR) / ROUNDING_FACTOR;
    }
} // namespace ShapeCorners

ShapeCorners::WindowConfig ShapeCorners::WindowConfig::activeWindowConfig()
{
    const QPalette &m_palette = m_widget.palette();
    WindowConfig    config    = {
                  .cornerRadius      = static_cast<float>(Config::size()),
                  .shadowSize        = static_cast<float>(Config::shadowSize()),
                  .outlineSize       = static_cast<float>(Config::outlineThickness()),
                  .secondOutlineSize = static_cast<float>(Config::secondOutlineThickness()),
                  .shadowColor       = FloatColor(
                    Config::activeShadowUsePalette()
                            ? m_palette.color(QPalette::Active,
                                                          static_cast<QPalette::ColorRole>(Config::activeShadowPalette()))
                            : Config::shadowColor()),
                  .outlineColor = FloatColor(
                    Config::activeOutlineUsePalette()
                            ? m_palette.color(QPalette::Active,
                                                    static_cast<QPalette::ColorRole>(Config::activeOutlinePalette()))
                            : Config::outlineColor()),
                  .secondOutlineColor = FloatColor(
                    Config::activeSecondOutlineUsePalette()
                            ? m_palette.color(QPalette::Active,
                                                    static_cast<QPalette::ColorRole>(Config::activeSecondOutlinePalette()))
                            : Config::secondOutlineColor()),
    };
    config.shadowColor.setAlpha(Config::activeShadowAlpha());
    config.outlineColor.setAlpha(Config::activeOutlineAlpha());
    config.secondOutlineColor.setAlpha(Config::activeSecondOutlineAlpha());
    return config;
}

ShapeCorners::WindowConfig ShapeCorners::WindowConfig::inactiveWindowConfig()
{
    const QPalette &m_palette = m_widget.palette();
    WindowConfig    config    = {
                  .cornerRadius      = static_cast<float>(Config::inactiveCornerRadius()),
                  .shadowSize        = static_cast<float>(Config::inactiveShadowSize()),
                  .outlineSize       = static_cast<float>(Config::inactiveOutlineThickness()),
                  .secondOutlineSize = static_cast<float>(Config::inactiveSecondOutlineThickness()),
                  .shadowColor       = FloatColor(
                    Config::inactiveShadowUsePalette()
                            ? m_palette.color(QPalette::Inactive,
                                                          static_cast<QPalette::ColorRole>(Config::inactiveShadowPalette()))
                            : Config::inactiveShadowColor()),
                  .outlineColor = FloatColor(
                    Config::inactiveOutlineUsePalette()
                            ? m_palette.color(QPalette::Inactive,
                                                    static_cast<QPalette::ColorRole>(Config::inactiveOutlinePalette()))
                            : Config::inactiveOutlineColor()),
                  .secondOutlineColor = FloatColor(
                    Config::inactiveSecondOutlineUsePalette()
                            ? m_palette.color(QPalette::Inactive,
                                                    static_cast<QPalette::ColorRole>(Config::inactiveSecondOutlinePalette()))
                            : Config::inactiveSecondOutlineColor()),
    };
    config.shadowColor.setAlpha(Config::inactiveShadowAlpha());
    config.outlineColor.setAlpha(Config::inactiveOutlineAlpha());
    config.secondOutlineColor.setAlpha(Config::inactiveSecondOutlineAlpha());
    return config;
}

ShapeCorners::WindowConfig ShapeCorners::WindowConfig::operator+(const WindowConfig &other) const
{
    return WindowConfig{.cornerRadius       = cornerRadius + other.cornerRadius,
                        .shadowSize         = shadowSize + other.shadowSize,
                        .outlineSize        = outlineSize + other.outlineSize,
                        .secondOutlineSize  = secondOutlineSize + other.secondOutlineSize,
                        .shadowColor        = shadowColor + other.shadowColor,
                        .outlineColor       = outlineColor + other.outlineColor,
                        .secondOutlineColor = secondOutlineColor + other.secondOutlineColor};
}

ShapeCorners::WindowConfig ShapeCorners::WindowConfig::operator-(const WindowConfig &other) const
{
    return WindowConfig{.cornerRadius       = cornerRadius - other.cornerRadius,
                        .shadowSize         = shadowSize - other.shadowSize,
                        .outlineSize        = outlineSize - other.outlineSize,
                        .secondOutlineSize  = secondOutlineSize - other.secondOutlineSize,
                        .shadowColor        = shadowColor - other.shadowColor,
                        .outlineColor       = outlineColor - other.outlineColor,
                        .secondOutlineColor = secondOutlineColor - other.secondOutlineColor};
}

ShapeCorners::WindowConfig ShapeCorners::WindowConfig::operator*(const float scalar) const
{
    return WindowConfig{.cornerRadius       = cornerRadius * scalar,
                        .shadowSize         = shadowSize * scalar,
                        .outlineSize        = outlineSize * scalar,
                        .secondOutlineSize  = secondOutlineSize * scalar,
                        .shadowColor        = shadowColor * scalar,
                        .outlineColor       = outlineColor * scalar,
                        .secondOutlineColor = secondOutlineColor * scalar};
}

ShapeCorners::WindowConfig ShapeCorners::WindowConfig::operator/(const float scalar) const
{
    return WindowConfig{.cornerRadius       = cornerRadius / scalar,
                        .shadowSize         = shadowSize / scalar,
                        .outlineSize        = outlineSize / scalar,
                        .secondOutlineSize  = secondOutlineSize / scalar,
                        .shadowColor        = shadowColor / scalar,
                        .outlineColor       = outlineColor / scalar,
                        .secondOutlineColor = secondOutlineColor / scalar};
}

bool ShapeCorners::WindowConfig::operator!() const
{
    return cornerRadius <= 0 && shadowSize <= 0 && outlineSize <= 0 && secondOutlineSize <= 0 && !shadowColor &&
           !outlineColor && !secondOutlineColor;
}

void ShapeCorners::WindowConfig::operator+=(const WindowConfig &other)
{
    cornerRadius += other.cornerRadius;
    shadowSize += other.shadowSize;
    outlineSize += other.outlineSize;
    secondOutlineSize += other.secondOutlineSize;
    shadowColor += other.shadowColor;
    outlineColor += other.outlineColor;
    secondOutlineColor += other.secondOutlineColor;
}

void ShapeCorners::WindowConfig::round()
{
    cornerRadius      = ShapeCorners::round(cornerRadius);
    shadowSize        = ShapeCorners::round(shadowSize);
    outlineSize       = ShapeCorners::round(outlineSize);
    secondOutlineSize = ShapeCorners::round(secondOutlineSize);
    shadowColor.round();
    outlineColor.round();
    secondOutlineColor.round();
}

void ShapeCorners::WindowConfig::clamp(const WindowConfig &direction, const WindowConfig &destination)
{
    cornerRadius = ShapeCorners::clamp(cornerRadius, direction.cornerRadius, destination.cornerRadius);
    shadowSize   = ShapeCorners::clamp(shadowSize, direction.shadowSize, destination.shadowSize);
    outlineSize  = ShapeCorners::clamp(outlineSize, direction.outlineSize, destination.outlineSize);
    secondOutlineSize =
            ShapeCorners::clamp(secondOutlineSize, direction.secondOutlineSize, destination.secondOutlineSize);
    shadowColor.clamp();
    outlineColor.clamp();
    secondOutlineColor.clamp();
}
