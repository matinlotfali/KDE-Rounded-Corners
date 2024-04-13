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

ShapeCornersWindow::ShapeCornersWindow(KWin::EffectWindow *w, const QString& name)
        : w(w), name(name)
{
}

bool ShapeCornersWindow::isActive() const {
    return KWin::effects->activeWindow() == w;
}

bool ShapeCornersWindow::hasEffect() const {
    return (w->isNormalWindow()
            || w->isDialog()
            || ShapeCornersConfig::inclusions().contains(name))
           && !ShapeCornersConfig::exclusions().contains(name);
}

bool ShapeCornersWindow::hasRoundCorners() const {
    return !(isTiled && ShapeCornersConfig::disableRoundTile())
        && !(isMaximized && ShapeCornersConfig::disableRoundMaximize());
}

bool ShapeCornersWindow::hasOutline() const {
    return !(isTiled && ShapeCornersConfig::disableOutlineTile())
        && !(isMaximized && ShapeCornersConfig::disableOutlineMaximize());
}
