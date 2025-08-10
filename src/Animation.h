//
// Created by matin on 8/3/25.
//

#pragma once

#include <QObject>
#include <chrono>
#include "WindowConfig.h"

namespace KWin
{
    class EffectWindow;
}

namespace ShapeCorners
{
    class Window;

    /**
     * @brief Manages animation state and configuration for window corner effects.
     *
     * Handles the transition between active and inactive window configurations,
     * updating animation progress and responding to window activation changes.
     */
    class Animation final : public QObject
    {
        Q_OBJECT

    public:
        /**
         * @brief Initializes the animation state and connects activation signals.
         */
        Animation();

        /**
         * @brief Gets the current frame configuration for a window.
         * @param window The window to query.
         * @return Pointer to the appropriate WindowConfig for the window's state.
         */
        [[nodiscard]]
        const WindowConfig *getFrameConfig(const Window &window) const;

        /**
         * @brief Gets the current configuration for the active window.
         * @return Pointer to the active WindowConfig.
         */
        [[nodiscard]]
        const WindowConfig *getActiveConfig() const
        {
            return &activeAnimation;
        }

        /**
         * @brief Gets the current configuration for inactive windows.
         * @return Pointer to the inactive WindowConfig.
         */
        [[nodiscard]]
        const WindowConfig *getInactiveConfig() const
        {
            return &inactiveAnimation;
        }

        /**
         * @brief Checks if an animation is currently in progress.
         * @return True if animating, false otherwise.
         */
        [[nodiscard]]
        bool isAnimating() const
        {
            return m_isAnimating;
        }

        /**
         * @brief Updates the animation state and interpolates configurations.
         *
         * Should be called regularly to progress the animation.
         */
        void update();

    private Q_SLOTS:
        /**
         * @brief Handles changes to the active window.
         * @param w The newly activated EffectWindow.
         */
        void setActiveWindowChanged(const KWin::EffectWindow *w);

    private:
        /// Duration remaining for the current animation, in milliseconds.
        long lastAnimationDuration;

        /// Whether an animation is currently running.
        bool m_isAnimating = true;

        /// Timestamp of the last active window change.
        std::chrono::system_clock::time_point lastActiveWindowChangedTime;

        /// Interpolated configuration for the active window.
        WindowConfig activeAnimation;

        /// Interpolated configuration for inactive windows.
        WindowConfig inactiveAnimation;
    };

} // namespace ShapeCorners
