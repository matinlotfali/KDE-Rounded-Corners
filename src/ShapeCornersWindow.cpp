//
// Created by matin on 10/04/24.
//

#include "ShapeCornersWindow.h"
#include "ShapeCornersConfig.h"

#include <qconfig.h>
#if QT_VERSION_MAJOR >= 6
#include <effect/effecthandler.h>
#else
#include <kwineffects.h>
#endif

QWidget ShapeCornersWindow::m_widget {};

ShapeCornersWindow::ShapeCornersWindow(KWin::EffectWindow *w, const QString& name)
        : w(w), name(name)
{ }

bool ShapeCornersWindow::isActive() const {
    return KWin::effects->activeWindow() == w;
}

bool ShapeCornersWindow::hasEffect() const {
    return (
        (
            (w->isNormalWindow() && ShapeCornersConfig::includeNormalWindows())
            || (w->isDialog() && ShapeCornersConfig::includeDialogs())
            || ShapeCornersConfig::inclusions().contains(name)
        )
        && !ShapeCornersConfig::exclusions().contains(name)
    );
}

bool ShapeCornersWindow::hasRoundCorners() const {
    return !(isTiled && ShapeCornersConfig::disableRoundTile() && !isMaximized)
        && !(isMaximized && ShapeCornersConfig::disableRoundMaximize());
}

bool ShapeCornersWindow::hasOutline() const {
    return !(isTiled && ShapeCornersConfig::disableOutlineTile() && !isMaximized)
        && !(isMaximized && ShapeCornersConfig::disableOutlineMaximize());
}

constexpr void clamp(float& value, const float& delta, const float& config) {
    if (delta > 0)
        value = std::min(value, config);
    else if (delta < 0)
        value = std::max(value, 0.0f);
}

bool ShapeCornersWindow::animateProperties(std::chrono::milliseconds time) {
    auto deltaTime = static_cast<float>((time - m_last_time).count());
    m_last_time = time;

    // find the destination value
    float configCornerRadius;
    float configShadowSize;
    float configOutlineSize;
    ShapeCornersColor configShadowColor;
    ShapeCornersColor configOutlineColor;
    auto& m_palette = m_widget.palette();
    if (isActive()) {
        configCornerRadius = static_cast<float>(ShapeCornersConfig::size());
        configShadowSize = static_cast<float>(ShapeCornersConfig::shadowSize());
        configOutlineSize = static_cast<float>(ShapeCornersConfig::outlineThickness());

        configShadowColor = ShapeCornersColor(ShapeCornersConfig::activeShadowUsePalette() ?
          m_palette.color(QPalette::Active, static_cast<QPalette::ColorRole>(ShapeCornersConfig::activeShadowPalette())):
          ShapeCornersConfig::shadowColor());
        configShadowColor.setAlpha(ShapeCornersConfig::activeShadowAlpha());

        configOutlineColor = ShapeCornersColor(ShapeCornersConfig::activeOutlineUsePalette() ?
                       m_palette.color(QPalette::Active, static_cast<QPalette::ColorRole>(ShapeCornersConfig::activeOutlinePalette())):
                       ShapeCornersConfig::outlineColor());
        configOutlineColor.setAlpha(hasOutline() ? ShapeCornersConfig::activeOutlineAlpha(): 0);
    } else {
        configCornerRadius = static_cast<float>(ShapeCornersConfig::inactiveCornerRadius());
        configShadowSize = static_cast<float>(ShapeCornersConfig::inactiveShadowSize());
        configOutlineSize = static_cast<float>(ShapeCornersConfig::inactiveOutlineThickness());

        configShadowColor = ShapeCornersColor(ShapeCornersConfig::inactiveShadowUsePalette() ?
                      m_palette.color(QPalette::Inactive, static_cast<QPalette::ColorRole>(ShapeCornersConfig::inactiveShadowPalette())):
                      ShapeCornersConfig::inactiveShadowColor());
        configShadowColor.setAlpha(ShapeCornersConfig::inactiveShadowAlpha());

        configOutlineColor = ShapeCornersColor(ShapeCornersConfig::inactiveOutlineUsePalette() ?
                       m_palette.color(QPalette::Inactive, static_cast<QPalette::ColorRole>(ShapeCornersConfig::inactiveOutlinePalette())):
                       ShapeCornersConfig::inactiveOutlineColor());
        configOutlineColor.setAlpha(hasOutline() ? ShapeCornersConfig::inactiveOutlineAlpha(): 0);
    }

    // calculate the animation step
    auto deltaCornerRadius = (configCornerRadius - cornerRadius)*5 / deltaTime;
    auto deltaShadowSize = (configShadowSize - shadowSize)*5 / deltaTime;
    auto deltaOutlineSize = (configOutlineSize - outlineSize)*5 / deltaTime;
    auto deltaShadowColor = (configShadowColor - shadowColor) / deltaTime;
    auto deltaOutlineColor = (configOutlineColor - outlineColor) / deltaTime;

    // adjust decimal precision
    deltaCornerRadius = std::round(deltaCornerRadius * 100) / 100;
    deltaShadowSize = std::round(deltaShadowSize * 100) / 100;
    deltaOutlineSize = std::round(deltaOutlineSize * 100) / 100;
    deltaShadowColor.round(3);
    deltaOutlineColor.round(3);

    // adjust properties
    cornerRadius += deltaCornerRadius;
    shadowSize += deltaShadowSize;
    outlineSize += deltaOutlineSize;
    shadowColor += deltaShadowColor;
    outlineColor += deltaOutlineColor;

    // check boundaries after adjusting
    clamp(cornerRadius, deltaCornerRadius, configCornerRadius);
    clamp(shadowSize, deltaShadowSize, configShadowSize);
    clamp(outlineSize, deltaOutlineSize, configOutlineSize);
    shadowColor.clamp();
    outlineColor.clamp();

    return (
        deltaShadowSize != 0
        || !deltaShadowColor.isZero()
        || !deltaOutlineColor.isZero()
    );
}
