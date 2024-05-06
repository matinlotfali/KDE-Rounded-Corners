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
#include <KDecoration2/Decoration>
#include <KDecoration2/DecorationShadow>

ShapeCornersWindow::ShapeCornersWindow(KWin::EffectWindow *w, const QString& name)
        : w(w), name(name)
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

QColor ShapeCornersWindow::getShadowColor() const {
    QColor result = Qt::transparent;
    auto decoration = w->decoration();
    if (decoration) {
        auto shadow = decoration->shadow().get();
        if (shadow) {
            auto shadowImage = shadow->shadow();
            result = shadowImage.pixelColor(135, 54);
        }
    }
    return result;
}
