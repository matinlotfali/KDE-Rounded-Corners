/**
 * @file Window.h
 * @brief Declares the ShapeCorners::Window class for managing window properties and animation.
 *
 * This file provides the Window class, which manages window state, appearance, and animation
 * for the ShapeCorners KWin effect.
 */

#pragma once

#include <QObject>
#include <QString>
#include <chrono>

#include "WindowConfig.h"
#ifdef QT_DEBUG
#include <QDebug>
#endif

class QWidget;

namespace KWin
{
    class EffectWindow;

#ifdef QT_DEBUG
    /**
     * @brief Debug operator for EffectWindow.
     */
    QDebug operator<<(const QDebug &debug, const EffectWindow &kwindow);
#endif
} // namespace KWin

namespace ShapeCorners
{
    struct WindowConfig;

    /**
     * @class Window
     * @brief Manages the state, appearance, and animation of a KWin window for ShapeCorners.
     *
     * Stores and animates properties such as corner radius, shadow, and outline for a managed window.
     * Handles inclusion/exclusion logic and provides utility methods for window state.
     */
    class Window final : public QObject
    {
        Q_OBJECT

    public:
        /**
         * @brief Reference to the managed KWin EffectWindow.
         */
        KWin::EffectWindow *w;

        /**
         * @brief True if the window is tiled.
         */
        bool isTiled = false;

        /**
         * @brief True if the window is maximized.
         */
        bool isMaximized = false;

        /// Duration remaining for the current animation, in milliseconds.
        long lastAnimationDuration;

        /// Whether an animation is currently running.
        bool isAnimating = true;

        /// Timestamp of the last active window change.
        std::chrono::system_clock::time_point lastActiveChangedTime;

        /**
         * @brief Current window configuration that may be animated.
         */
        WindowConfig currentConfig;

        /**
         * @brief Constructs a Window object for the given EffectWindow.
         * @param kwindow Reference to the KWin EffectWindow.
         */
        explicit Window(KWin::EffectWindow *kwindow);

        ~Window() = default;

        // don't allow copying
        Window(const Window &)  = delete;
        Window(const Window &&) = delete;

        Window &operator=(const Window &)  = delete;
        Window &operator=(const Window &&) = delete;

        /**
         * @brief Checks if the window is the currently active window.
         * @return True if the window is active.
         */
        [[nodiscard]]
        bool isActive() const;

        /**
         * @brief Checks if the window should have rounded corners.
         * @return True if the window has rounded corners.
         */
        [[nodiscard]]
        bool hasRoundCorners() const;

        /**
         * @brief Checks if the window should have an outline.
         * @return True if the window has an outline.
         */
        [[nodiscard]]
        bool hasOutline() const;

        /**
         * @brief Checks if the window should have any ShapeCorners effect.
         * @return True if the window should be affected by the effect.
         */
        [[nodiscard]]
        bool hasEffect() const;

        /**
         * @brief Serializes window information to JSON.
         * @return QJsonObject containing window class and caption.
         */
        [[nodiscard]]
        QJsonObject toJson() const;

        /**
         *  @brief Returns the windows caption after the " — ".
         *
         *  This is useful for apps like Dolphin and Konsole that include the working file/directory in their title,
         *  and we don't want that to be considered in the inclusion and exclusion rules.
         *  For example a window with the title "file — Dolphin" will have "Dolphin" returned.
         *  @return The caption after the dash, or the full caption if not found.
         */
        [[nodiscard]]
        QString captionAfterDash() const;

    public Q_SLOTS:
        /**
         * @brief Updates inclusion/exclusion state when configuration changes.
         */
        void configChanged();

    private:
        /**
         * @brief True if the window is explicitly included by config.
         */
        bool isIncluded = false;

        /**
         * @brief True if the window is explicitly excluded by config.
         */
        bool isExcluded = false;
    };

#ifdef QT_DEBUG
    /**
     * @brief Debug operator for ShapeCorners::Window.
     */
    inline QDebug operator<<(const QDebug &debug, const Window &win) { return (debug << *win.w); }
#endif
} // namespace ShapeCorners
