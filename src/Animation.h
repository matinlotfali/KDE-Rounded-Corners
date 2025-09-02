//
// Created by matin on 8/3/25.
//

#pragma once

namespace ShapeCorners
{
    class Window;
    /**
     * @brief Manages animation state and configuration for window corner effects.
     *
     * Handles the transition between active and inactive window configurations,
     * updating animation progress and responding to window activation changes.
     */
    class Animation
    {
    public:
        /**
         * @brief Initializes the animation state and connects activation signals.
         */
        Animation();

        /**
         * @brief Updates the animation state and interpolates configurations.
         * @param window The Window object to update.
         * Should be called regularly to progress the animation.
         */
        void update(Window &window);

    private:
        /**
         * @brief Handles changes to the active window.
         * @param window The newly activated EffectWindow. Null if no window is active.
         */
        void setActiveWindowChanged(Window *window);

        /// Pointer to the current active window, only used to detect changes.
        Window *currentActiveWindow = nullptr;

        /// Pointer to the last active window, only used to detect changes.
        Window *lastActiveWindow = nullptr;
    };
} // namespace ShapeCorners
