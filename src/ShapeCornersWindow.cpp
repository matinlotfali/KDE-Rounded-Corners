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

bool ShapeCornersWindow::animateProperties(std::chrono::milliseconds time) {
    auto deltaTime = static_cast<float>((time - m_last_time).count());
    auto& m_palette = m_widget.palette();
    m_last_time = time;

    float deltaShadowSize;
    ShapeCornersColor deltaShadowColor;
    ShapeCornersColor deltaOutlineColor;

    float configShadowSize;
    ShapeCornersColor configShadowColor;
    ShapeCornersColor configOutlineColor;

    if (isActive()) {
        configShadowSize = static_cast<float>(ShapeCornersConfig::shadowSize());
        configShadowColor = ShapeCornersColor(ShapeCornersConfig::activeShadowUsePalette() ?
          m_palette.color(QPalette::Active, static_cast<QPalette::ColorRole>(ShapeCornersConfig::activeShadowPalette())):
          ShapeCornersConfig::shadowColor());
        configShadowColor.setAlpha(ShapeCornersConfig::activeShadowAlpha());

        configOutlineColor = ShapeCornersColor(ShapeCornersConfig::activeOutlineUsePalette() ?
                       m_palette.color(QPalette::Active, static_cast<QPalette::ColorRole>(ShapeCornersConfig::activeOutlinePalette())):
                       ShapeCornersConfig::outlineColor());
        configOutlineColor.setAlpha(hasOutline() ? ShapeCornersConfig::activeOutlineAlpha(): 0);
    } else {
        configShadowSize = static_cast<float>(ShapeCornersConfig::inactiveShadowSize());
        configShadowColor = ShapeCornersColor(ShapeCornersConfig::inactiveShadowUsePalette() ?
                      m_palette.color(QPalette::Inactive, static_cast<QPalette::ColorRole>(ShapeCornersConfig::inactiveShadowPalette())):
                      ShapeCornersConfig::inactiveShadowColor());
        configShadowColor.setAlpha(ShapeCornersConfig::inactiveShadowAlpha());

        configOutlineColor = ShapeCornersColor(ShapeCornersConfig::inactiveOutlineUsePalette() ?
                       m_palette.color(QPalette::Inactive, static_cast<QPalette::ColorRole>(ShapeCornersConfig::inactiveOutlinePalette())):
                       ShapeCornersConfig::inactiveOutlineColor());
        configOutlineColor.setAlpha(hasOutline() ? ShapeCornersConfig::inactiveOutlineAlpha(): 0);
    }

    deltaShadowSize = (configShadowSize - shadowSize)*5 / deltaTime;
    deltaShadowColor = (configShadowColor - shadowColor) / deltaTime;
    deltaOutlineColor = (configOutlineColor - outlineColor) / deltaTime;

    deltaShadowSize = std::round(deltaShadowSize * 100) / 100;
    deltaShadowColor.round(3);
    deltaOutlineColor.round(3);

    shadowSize += deltaShadowSize;
    shadowColor += deltaShadowColor;
    outlineColor += deltaOutlineColor;

    shadowSize = std::clamp(shadowSize, 0.0f, configShadowSize);
    shadowColor.clamp();
    outlineColor.clamp();

    return (
        deltaShadowSize != 0
        || !deltaShadowColor.isZero()
        || !deltaOutlineColor.isZero()
    );
}
