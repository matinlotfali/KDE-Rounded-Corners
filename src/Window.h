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

#ifdef QT_DEBUG
    QDebug operator<<(QDebug& debug, const EffectWindow& w);
    inline QDebug operator<<(QDebug& debug, const EffectWindow* w) { return (debug << *w); }
#endif
}

namespace ShapeCorners {
    class Window: public QObject {
        Q_OBJECT

    public:
        KWin::EffectWindow& w;
        bool isTiled = false;
        bool isMaximized = false;

        float cornerRadius;
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

        /**
         *  \brief Returns the windows caption after the " — ".
         *  \description This is useful for apps like Dolphin and Konsole that include the working file/directory in their title
         *  and we don't want that to be considered in the inclusion and exclusion rules.
         *  \example a window with the title "file — Dolphin" will have "Dolphin" returned.
         */
        [[nodiscard]] QString captionAfterDash() const;

    public Q_SLOTS:
        void configChanged();

    private:
        std::chrono::milliseconds m_last_time = {};
        bool isIncluded = false;
        bool isExcluded = false;

        /**
         * \brief Used only for its `palette()` function which holds the currently active highlight colors.
         */
        static QWidget m_widget;
    };

#ifdef QT_DEBUG
    inline QDebug operator<<(QDebug& debug, const Window& w) { return (debug << w.w); }
#endif
}

#endif //KWIN4_EFFECT_SHAPECORNERS_WINDOW_H
