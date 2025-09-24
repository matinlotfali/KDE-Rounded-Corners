/**
 * @file WindowConfig.h
 * @brief Declares the ShapeCorners::WindowConfig struct for storing window appearance configuration.
 *
 * This struct holds all configurable properties for a window's appearance in the ShapeCorners KWin effect,
 * including corner radius, shadow, and outline settings.
 */

#pragma once

#include "FloatColor.h"

namespace ShapeCorners
{
    /**
     * @struct WindowConfig
     * @brief Stores configuration for a window's appearance and animation.
     *
     * Holds values for corner radius, shadow size/color, outline thickness/color, and second outline.
     * Provides arithmetic operators and utility methods for animation and value clamping.
     */
    struct WindowConfig {
        /**
         * @brief Current corner radius.
         */
        float cornerRadius{};

        /**
         * @brief Current shadow size.
         */
        float shadowSize{};

        /**
         * @brief Current outline thickness.
         */
        float outlineSize{};

        /**
         * @brief Current second outline thickness.
         */
        float secondOutlineSize{};

        /**
         * @brief Current outer outline thickness.
         */
        float outerOutlineSize{};

        /**
         * @brief Current shadow color.
         */
        FloatColor shadowColor;

        /**
         * @brief Current outline color.
         */
        FloatColor outlineColor;

        /**
         * @brief Current second outline color.
         */
        FloatColor secondOutlineColor;

        /**
         * @brief Current outer outline color.
         */
        FloatColor outerOutlineColor;

        /**
         * @brief Returns the configuration for an active window.
         * @return WindowConfig for an active window.
         */
        static WindowConfig activeWindowConfig();

        /**
         * @brief Returns the configuration for an inactive window.
         * @return WindowConfig for an inactive window.
         */
        static WindowConfig inactiveWindowConfig();

        /**
         * @brief Adds two WindowConfig objects.
         * @param other The other WindowConfig.
         * @return The sum of the two WindowConfigs.
         */
        WindowConfig operator+(const WindowConfig &other) const;

        /**
         * @brief Subtracts another WindowConfig from this one.
         * @param other The other WindowConfig.
         * @return The difference of the two WindowConfigs.
         */
        WindowConfig operator-(const WindowConfig &other) const;

        /**
         * @brief Multiply all fields with a scalar.
         * @param scalar The scalar value.
         * @return The scaled WindowConfig.
         */
        WindowConfig operator*(float scalar) const;

        /**
         * @brief Divides all fields by a scalar.
         * @param scalar The scalar value.
         * @return The scaled WindowConfig.
         */
        WindowConfig operator/(float scalar) const;

        /**
         * @brief Checks if all fields are zero or default.
         * @return True if all fields are zero/default, false otherwise.
         */
        bool operator!() const;

        /**
         * @brief Adds another WindowConfig to this one in-place.
         * @param other The other WindowConfig.
         */
        void operator+=(const WindowConfig &other);

        /**
         * @brief Rounds all floating-point fields to the nearest integer.
         */
        void round();

        /**
         * @brief Clamps all fields to the range defined by direction and destination configs.
         * @param direction The direction of change for each field.
         * @param destination The destination config to clamp to.
         */
        void clamp(const WindowConfig &direction, const WindowConfig &destination);
    };
} // namespace ShapeCorners
