//
// Created by matin on 09/04/24.
//

#ifndef KWIN4_EFFECT_SHAPECORNERS_SHAPECORNERSWINDOW_H
#define KWIN4_EFFECT_SHAPECORNERS_SHAPECORNERSWINDOW_H

#include "ShapeCornersColor.h"

#include <QWidget>
#include <QString>
#include <chrono>
#include <memory>

class QWidget;

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

    float cornerRadius = 0;
    float shadowSize = 0;
    float outlineSize = 0;
    ShapeCornersColor shadowColor = {};
    ShapeCornersColor outlineColor = {};

    explicit ShapeCornersWindow(KWin::EffectWindow *w, const QString& name);

    [[nodiscard]] bool animateProperties(std::chrono::milliseconds time);

    [[nodiscard]] bool isActive() const;
    [[nodiscard]] bool hasRoundCorners() const;
    [[nodiscard]] bool hasOutline() const;
    [[nodiscard]] bool hasEffect() const;

private:
    std::chrono::milliseconds m_last_time;

    /**
     * \brief Used only for its `palette()` function which holds the currently active highlight colors.
     */
    static QWidget m_widget;
};

#endif //KWIN4_EFFECT_SHAPECORNERS_SHAPECORNERSWINDOW_H
