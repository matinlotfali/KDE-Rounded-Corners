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

    QDebug operator<<(QDebug& debug, const KWin::EffectWindow& w);
    inline QDebug operator<<(QDebug& debug, const KWin::EffectWindow* w) { return (debug << *w); }
}

namespace ShapeCorners {
    class Window: public QObject {
        Q_OBJECT

    public:
        KWin::EffectWindow& w;
        bool isTiled = false;
        bool isMaximized = false;

        float cornerRadius = -1;
        float shadowSize = -1;
        float outlineSize = -1;
        float secondOutlineSize = -1;
        Color shadowColor = {};
        Color outlineColor = {};
        Color secondOutlineColor = {};

#ifdef DEBUG_ANIMATION
        uint32_t repaintCount = 0;
#endif

        explicit Window(KWin::EffectWindow& w);

        void animateProperties(const std::chrono::milliseconds &time);

        [[nodiscard]] bool isActive() const;

        [[nodiscard]] bool hasRoundCorners() const;

        [[nodiscard]] bool hasOutline() const;

        [[nodiscard]] bool hasEffect() const;

        [[nodiscard]] QJsonObject toJson() const;

    public Q_SLOTS:
        void configChanged();

    private:
        std::chrono::milliseconds m_last_time = {};
        bool isIncluded;
        bool isExcluded;

        /**
         * \brief Used only for its `palette()` function which holds the currently active highlight colors.
         */
        static QWidget m_widget;
    };

    inline QDebug operator<<(QDebug& debug, const Window& w) { return (debug << w.w); }
}

#endif //KWIN4_EFFECT_SHAPECORNERS_WINDOW_H
