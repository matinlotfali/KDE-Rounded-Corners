/**
 * @file WindowConfig.h
 * @brief Declares the ShapeCorners::WindowConfig and ShapeCorners::OutlineConfig structs.
 *
 * These structs hold all configurable properties for a window's appearance in the ShapeCorners KWin effect,
 * including corner radius, shadow, and outline settings. OutlineConfig groups the per-outline properties
 * (thickness and a two-stop linear gradient) so they can be reused for each of the three outlines.
 */

#pragma once

#include "FloatColor.h"

namespace ShapeCorners
{
    /**
     * @struct OutlineConfig
     * @brief Stores one outline's thickness and its two-stop linear gradient.
     *
     * When the outline is a solid color, both gradient stops are equal. Provides the same
     * arithmetic operators as WindowConfig so active/inactive states can be interpolated per outline.
     */
    struct OutlineConfig {
        /**
         * @brief Outline thickness in pixels.
         */
        float size{};

        /**
         * @brief Gradient angle in degrees. 0 = left to right, 90 = top to bottom.
         */
        float angle{};

        /**
         * @brief First gradient stop color.
         */
        FloatColor color1;

        /**
         * @brief Second gradient stop color. Equal to color1 when the outline is a solid color.
         */
        FloatColor color2;

        /**
         * @brief Adds two OutlineConfig objects.
         */
        OutlineConfig operator+(const OutlineConfig &other) const;

        /**
         * @brief Subtracts another OutlineConfig from this one.
         */
        OutlineConfig operator-(const OutlineConfig &other) const;

        /**
         * @brief Multiplies all fields by a scalar.
         */
        OutlineConfig operator*(float scalar) const;

        /**
         * @brief Divides all fields by a scalar.
         */
        OutlineConfig operator/(float scalar) const;

        /**
         * @brief Adds another OutlineConfig to this one in-place.
         */
        void operator+=(const OutlineConfig &other);

        /**
         * @brief Checks whether this outline draws nothing (zero size and transparent stops).
         * @note The gradient angle is not considered; it does not affect whether anything is drawn.
         */
        [[nodiscard]]
        bool operator!() const;

        /**
         * @brief Rounds all floating-point fields.
         */
        void round();

        /**
         * @brief Clamps all fields to the range defined by direction and destination configs.
         */
        void clamp(const OutlineConfig &direction, const OutlineConfig &destination);
    };

    /**
     * @struct WindowConfig
     * @brief Stores configuration for a window's appearance and animation.
     *
     * Holds values for corner radius, shadow size/color, and the three outlines.
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
         * @brief Current shadow color.
         */
        FloatColor shadowColor;

        /**
         * @brief Current primary outline configuration.
         */
        OutlineConfig outline;

        /**
         * @brief Current second outline configuration.
         */
        OutlineConfig secondOutline;

        /**
         * @brief Current outer outline configuration.
         */
        OutlineConfig outerOutline;

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
