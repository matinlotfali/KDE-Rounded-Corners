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
#include <QWidget>

ShapeCornersWindow::ShapeCornersWindow(KWin::EffectWindow *w, const QString& name)
        : w(w), name(name),
          m_widget(new QWidget)
{
}

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
    auto& m_palette = m_widget->palette();
    m_last_time = time;

    float deltaShadowSize;
    ShapeCornersColor deltaShadowColor;
    ShapeCornersColor deltaOutlineColor;

    uint configShadowSize;
    ShapeCornersColor configShadowColor;
    ShapeCornersColor configOutlineColor;

    if (isActive()) {
        configShadowSize = ShapeCornersConfig::shadowSize();
        configShadowColor = ShapeCornersConfig::activeShadowUsePalette() ?
          m_palette.color(QPalette::Active, static_cast<QPalette::ColorRole>(ShapeCornersConfig::activeShadowPalette())):
          ShapeCornersConfig::shadowColor();
        configShadowColor.setAlpha(ShapeCornersConfig::activeShadowAlpha());

        configOutlineColor = ShapeCornersConfig::activeOutlineUsePalette() ?
                       m_palette.color(QPalette::Active, static_cast<QPalette::ColorRole>(ShapeCornersConfig::activeOutlinePalette())):
                       ShapeCornersConfig::outlineColor();
        configOutlineColor.setAlpha(hasOutline() ? ShapeCornersConfig::activeOutlineAlpha(): 0);
    } else {
        configShadowSize = ShapeCornersConfig::inactiveShadowSize();
        configShadowColor = ShapeCornersConfig::inactiveShadowUsePalette() ?
                      m_palette.color(QPalette::Inactive, static_cast<QPalette::ColorRole>(ShapeCornersConfig::inactiveShadowPalette())):
                      ShapeCornersConfig::inactiveShadowColor();
        configShadowColor.setAlpha(ShapeCornersConfig::inactiveShadowAlpha());

        configOutlineColor = ShapeCornersConfig::inactiveOutlineUsePalette() ?
                       m_palette.color(QPalette::Inactive, static_cast<QPalette::ColorRole>(ShapeCornersConfig::inactiveOutlinePalette())):
                       ShapeCornersConfig::inactiveOutlineColor();
        configOutlineColor.setAlpha(hasOutline() ? ShapeCornersConfig::inactiveOutlineAlpha(): 0);
    }

    deltaShadowSize = (static_cast<float>(configShadowSize) - shadowSize)*5 / deltaTime;
    deltaShadowColor = (configShadowColor - shadowColor) / deltaTime;
    deltaOutlineColor = (configOutlineColor - outlineColor) / deltaTime;

    shadowSize += deltaShadowSize;
    shadowColor += deltaShadowColor;
    outlineColor += deltaOutlineColor;

    return (
        std::round(deltaShadowSize*100) != 0
        && deltaShadowColor.isZero()
        && deltaOutlineColor.isZero()
    );
}
