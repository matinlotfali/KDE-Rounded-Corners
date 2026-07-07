#include "WindowConfig.h"
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
} // namespace

namespace ShapeCorners
{
    /**
     * \brief Used only for its `palette()` function which holds the currently active highlight colors.
     */
    const static QWidget m_widget{};
} // namespace ShapeCorners

ShapeCorners::WindowConfig ShapeCorners::WindowConfig::activeWindowConfig()
{
    const QPalette &m_palette = m_widget.palette();
    WindowConfig    config    = {
                  .cornerRadius      = static_cast<float>(Config::size()),
                  .shadowSize        = static_cast<float>(Config::shadowSize()),
                  .outlineSize       = static_cast<float>(Config::outlineThickness()),
                  .secondOutlineSize = static_cast<float>(Config::secondOutlineThickness()),
                  .outerOutlineSize  = static_cast<float>(Config::outerOutlineThickness()),
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
                  .outerOutlineColor = FloatColor(
                    Config::activeOuterOutlineUsePalette()
                            ? m_palette.color(QPalette::Active,
                                                    static_cast<QPalette::ColorRole>(Config::activeOuterOutlinePalette()))
                            : Config::outerOutlineColor()),
    };
    config.shadowColor.setAlpha(Config::activeShadowAlpha());
    config.outlineColor.setAlpha(Config::activeOutlineAlpha());
    config.secondOutlineColor.setAlpha(Config::activeSecondOutlineAlpha());
    config.outerOutlineColor.setAlpha(Config::activeOuterOutlineAlpha());
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
                  .outerOutlineSize  = static_cast<float>(Config::inactiveOuterOutlineThickness()),
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
                  .outerOutlineColor = FloatColor(
                    Config::inactiveOuterOutlineUsePalette()
                            ? m_palette.color(QPalette::Inactive,
                                                    static_cast<QPalette::ColorRole>(Config::inactiveOuterOutlinePalette()))
                            : Config::inactiveOuterOutlineColor()),
    };
    config.shadowColor.setAlpha(Config::inactiveShadowAlpha());
    config.outlineColor.setAlpha(Config::inactiveOutlineAlpha());
    config.secondOutlineColor.setAlpha(Config::inactiveSecondOutlineAlpha());
    config.outerOutlineColor.setAlpha(Config::inactiveOuterOutlineAlpha());
    return config;
}

ShapeCorners::WindowConfig ShapeCorners::WindowConfig::operator+(const WindowConfig &other) const
{
    return WindowConfig{.cornerRadius       = cornerRadius + other.cornerRadius,
                        .shadowSize         = shadowSize + other.shadowSize,
                        .outlineSize        = outlineSize + other.outlineSize,
                        .secondOutlineSize  = secondOutlineSize + other.secondOutlineSize,
                        .outerOutlineSize   = outerOutlineSize + other.outerOutlineSize,
                        .shadowColor        = shadowColor + other.shadowColor,
                        .outlineColor       = outlineColor + other.outlineColor,
                        .secondOutlineColor = secondOutlineColor + other.secondOutlineColor,
                        .outerOutlineColor  = outerOutlineColor + other.outerOutlineColor};
}

ShapeCorners::WindowConfig ShapeCorners::WindowConfig::operator-(const WindowConfig &other) const
{
    return WindowConfig{.cornerRadius       = cornerRadius - other.cornerRadius,
                        .shadowSize         = shadowSize - other.shadowSize,
                        .outlineSize        = outlineSize - other.outlineSize,
                        .secondOutlineSize  = secondOutlineSize - other.secondOutlineSize,
                        .outerOutlineSize   = outerOutlineSize - other.outerOutlineSize,
                        .shadowColor        = shadowColor - other.shadowColor,
                        .outlineColor       = outlineColor - other.outlineColor,
                        .secondOutlineColor = secondOutlineColor - other.secondOutlineColor,
                        .outerOutlineColor  = outerOutlineColor - other.outerOutlineColor};
}

ShapeCorners::WindowConfig ShapeCorners::WindowConfig::operator*(const float scalar) const
{
    return WindowConfig{.cornerRadius       = cornerRadius * scalar,
                        .shadowSize         = shadowSize * scalar,
                        .outlineSize        = outlineSize * scalar,
                        .secondOutlineSize  = secondOutlineSize * scalar,
                        .outerOutlineSize   = outerOutlineSize * scalar,
                        .shadowColor        = shadowColor * scalar,
                        .outlineColor       = outlineColor * scalar,
                        .secondOutlineColor = secondOutlineColor * scalar,
                        .outerOutlineColor  = outerOutlineColor * scalar};
}

ShapeCorners::WindowConfig ShapeCorners::WindowConfig::operator/(const float scalar) const
{
    return WindowConfig{.cornerRadius       = cornerRadius / scalar,
                        .shadowSize         = shadowSize / scalar,
                        .outlineSize        = outlineSize / scalar,
                        .secondOutlineSize  = secondOutlineSize / scalar,
                        .outerOutlineSize   = outerOutlineSize / scalar,
                        .shadowColor        = shadowColor / scalar,
                        .outlineColor       = outlineColor / scalar,
                        .secondOutlineColor = secondOutlineColor / scalar,
                        .outerOutlineColor  = outerOutlineColor / scalar};
}

bool ShapeCorners::WindowConfig::operator!() const
{
    return cornerRadius <= 0 && shadowSize <= 0 && outlineSize <= 0 && secondOutlineSize <= 0 &&
           outerOutlineSize <= 0 && !shadowColor && !outlineColor && !secondOutlineColor && !outerOutlineColor;
}

void ShapeCorners::WindowConfig::operator+=(const WindowConfig &other)
{
    cornerRadius += other.cornerRadius;
    shadowSize += other.shadowSize;
    outlineSize += other.outlineSize;
    secondOutlineSize += other.secondOutlineSize;
    outerOutlineSize += other.outerOutlineSize;
    shadowColor += other.shadowColor;
    outlineColor += other.outlineColor;
    secondOutlineColor += other.secondOutlineColor;
    outerOutlineColor += other.outerOutlineColor;
}

void ShapeCorners::WindowConfig::round()
{
    cornerRadius      = ::round_two_decimal(cornerRadius);
    shadowSize        = ::round_two_decimal(shadowSize);
    outlineSize       = ::round_two_decimal(outlineSize);
    secondOutlineSize = ::round_two_decimal(secondOutlineSize);
    outerOutlineSize  = ::round_two_decimal(outerOutlineSize);
    shadowColor.round();
    outlineColor.round();
    secondOutlineColor.round();
    outerOutlineColor.round();
}

void ShapeCorners::WindowConfig::clamp(const WindowConfig &direction, const WindowConfig &destination)
{
    cornerRadius      = ::clamp(cornerRadius, direction.cornerRadius, destination.cornerRadius);
    shadowSize        = ::clamp(shadowSize, direction.shadowSize, destination.shadowSize);
    outlineSize       = ::clamp(outlineSize, direction.outlineSize, destination.outlineSize);
    secondOutlineSize = ::clamp(secondOutlineSize, direction.secondOutlineSize, destination.secondOutlineSize);
    outerOutlineSize  = ::clamp(outerOutlineSize, direction.outerOutlineSize, destination.outerOutlineSize);
    shadowColor.clamp();
    outlineColor.clamp();
    secondOutlineColor.clamp();
    outerOutlineColor.clamp();
}
