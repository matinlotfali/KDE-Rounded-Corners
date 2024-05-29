//
// Created by matin on 09/04/24.
//

#ifndef KWIN4_EFFECT_SHAPECORNERS_WINDOW_H
#define KWIN4_EFFECT_SHAPECORNERS_WINDOW_H

#include "Color.h"

#include <QWidget>
#include <QString>
#include <chrono>
#include <memory>

class QWidget;

namespace KWin
{
    class EffectWindow;
}

namespace ShapeCorners {
    struct Window {
        KWin::EffectWindow *w;
        QString name;
        bool isTiled = false;
        bool isMaximized = false;

        float cornerRadius = -1;
        float shadowSize = -1;
        float outlineSize = -1;
        float secondOutlineSize = -1;
        Color shadowColor = {};
        Color outlineColor = {};
        Color secondOutlineColor = {};

#ifdef QT_DEBUG
        uint32_t repaintCount = 0;
#endif

        explicit Window(KWin::EffectWindow *w, QString name);

        void animateProperties(const std::chrono::milliseconds &time);

        [[nodiscard]] bool isActive() const;

        [[nodiscard]] bool hasRoundCorners() const;

        [[nodiscard]] bool hasOutline() const;

        [[nodiscard]] bool hasEffect() const;

    private:
        std::chrono::milliseconds m_last_time = {};

        /**
         * \brief Used only for its `palette()` function which holds the currently active highlight colors.
         */
        static QWidget m_widget;

        [[nodiscard]] QColor getDecorationShadowColor() const;
    };
}

#endif //KWIN4_EFFECT_SHAPECORNERS_WINDOW_H
