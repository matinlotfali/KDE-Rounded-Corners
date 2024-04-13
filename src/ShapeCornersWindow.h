//
// Created by matin on 09/04/24.
//

#ifndef KWIN4_EFFECT_SHAPECORNERS_SHAPECORNERSWINDOW_H
#define KWIN4_EFFECT_SHAPECORNERS_SHAPECORNERSWINDOW_H

#include "QString"

namespace KWin
{
    class EffectWindow;
}

struct ShapeCornersWindow
{
    KWin::EffectWindow *w;
    QString name;
    bool isTiled = false;
    bool isMaximized = false;

    explicit ShapeCornersWindow(KWin::EffectWindow *w, const QString& name);

    [[nodiscard]] bool isActive() const;
    [[nodiscard]] bool hasRoundCorners() const;
    [[nodiscard]] bool hasOutline() const;
    [[nodiscard]] bool hasEffect() const;
};

#endif //KWIN4_EFFECT_SHAPECORNERS_SHAPECORNERSWINDOW_H
