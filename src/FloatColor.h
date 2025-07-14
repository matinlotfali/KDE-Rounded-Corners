/**
 * @file FloatColor.h
 * @brief Defines the ShapeCorners::FloatColor struct for RGBA color manipulation with floating-point precision.
 *
 * This file provides the FloatColor struct, which supports arithmetic operations, conversion to/from QColor,
 * and utility methods for manipulating color channels.
 *
 * @author matin
 * @date 07/05/24
 */

#pragma once

#include <QColor>
#include <math.h>
#ifdef QT_DEBUG
#include <sstream>
#endif

namespace ShapeCorners {
    /**
     * @brief Represents a color with floating-point RGBA channels.
     *
     * Provides arithmetic operations, conversion to/from QColor, and utility methods
     * for manipulating color channels.
     */
    struct FloatColor {

        float r;
        float g;
        float b;
        float a;
        constexpr static float MIN_CHANNEL_VALUE = 0.0F;
        constexpr static float MAX_CHANNEL_VALUE = 255.0F;

        /**
         * @brief Default constructor. Initializes all channels to MIN_CHANNEL_VALUE.
         */
        constexpr FloatColor() :
                FloatColor(MIN_CHANNEL_VALUE, MIN_CHANNEL_VALUE, MIN_CHANNEL_VALUE, MIN_CHANNEL_VALUE) {}

        /**
         * @brief Constructs a FloatColor from a QColor.
         * @param color The QColor to convert.
         */
        explicit FloatColor(const QColor &color) :
                FloatColor(color.red(), color.green(), color.blue(), color.alpha()) {}

        /**
         * @brief Constructs a FloatColor from integer RGBA values.
         * @param red Red channel [0, 255].
         * @param green Green channel [0, 255].
         * @param blue Blue channel [0, 255].
         * @param alpha Alpha channel [0, 255], defaults to MAX_CHANNEL_VALUE.
         */
        constexpr FloatColor(const int red, const int green, const int blue, const int alpha = MAX_CHANNEL_VALUE) :
                r(static_cast<float>(red)),
                g(static_cast<float>(green)),
                b(static_cast<float>(blue)),
                a(static_cast<float>(alpha)) {}

        /**
         * @brief Constructs a FloatColor from float RGBA values.
         * @param red Red channel.
         * @param green Green channel.
         * @param blue Blue channel.
         * @param alpha Alpha channel, defaults to MAX_CHANNEL_VALUE.
         */
        constexpr FloatColor(const float red, const float green, const float blue,
                             const float alpha = MAX_CHANNEL_VALUE) :
                r(red), g(green), b(blue), a(alpha) {}

        /**
         * @brief Subtracts another FloatColor from this one.
         * @param other The color to subtract.
         * @return The result of the subtraction.
         */
        constexpr FloatColor operator-(const FloatColor &other) const noexcept {
            return {
                r - other.r,
                g - other.g,
                b - other.b,
                a - other.a
            };
        }

        /**
         * @brief Adds another FloatColor to this one.
         * @param other The color to add.
         * @return The result of the addition.
         */
        constexpr FloatColor operator+(const FloatColor &other) const noexcept {
            return {
                r + other.r,
                g + other.g,
                b + other.b,
                a + other.a
            };
        }

        /**
         * @brief Multiplies all channels by a scalar.
         * @tparam T Scalar type.
         * @param scalar The scalar value.
         * @return The result of the multiplication.
         */
        template<typename T>
        constexpr FloatColor operator*(const T &scalar) const noexcept {
            return {
                r * scalar,
                g * scalar,
                b * scalar,
                a * scalar
            };
        }

        /**
         * @brief Divides all channels by a scalar.
         * @tparam T Scalar type.
         * @param scalar The scalar value.
         * @return The result of the division.
         */
        template<typename T>
        constexpr FloatColor operator/(const T &scalar) const noexcept {
            return {
                r / scalar,
                g / scalar,
                b / scalar,
                a / scalar
            };
        }

        /**
         * @brief Checks if all channels are zero.
         * @return True if all channels are zero, false otherwise.
         */
        [[nodiscard]]
        constexpr bool operator!() const noexcept {
            return (r <= 0 && g <= 0 && b <= 0 && a <= 0);
        }

        /**
         * @brief Converts this FloatColor to a QColor.
         * @return The QColor representation.
         */
        [[nodiscard]]
        QColor toQColor() const noexcept {
            return {
                static_cast<int>(r),
                static_cast<int>(g),
                static_cast<int>(b),
                static_cast<int>(a)
            };
        }

#ifdef QT_DEBUG
        /**
         * @brief Returns a string representation of the color.
         * @return The string representation.
         */
        [[nodiscard]]
        std::string toString() const noexcept {
            std::stringstream stream;
            stream << "[ " << r << " , " << g << " , " << b << " , " << a << " ]";
            return stream.str();
        }
#endif

        /**
         * @brief Adds another FloatColor to this one in-place.
         * @param other The color to add.
         */
        constexpr void operator+=(const FloatColor &other) noexcept {
            r += other.r;
            g += other.g;
            b += other.b;
            a += other.a;
        }

        /**
         * @brief Rounds all channels to the nearest integer.
         */
        constexpr void round() noexcept {
            r = std::round(r);
            g = std::round(g);
            b = std::round(b);
            a = std::round(a);
        }

        /**
         * @brief Clamps all channels to the valid range [MIN_CHANNEL_VALUE, MAX_CHANNEL_VALUE].
         */
        constexpr void clamp() noexcept {
            r = std::clamp(r, MIN_CHANNEL_VALUE, MAX_CHANNEL_VALUE);
            g = std::clamp(g, MIN_CHANNEL_VALUE, MAX_CHANNEL_VALUE);
            b = std::clamp(b, MIN_CHANNEL_VALUE, MAX_CHANNEL_VALUE);
            a = std::clamp(a, MIN_CHANNEL_VALUE, MAX_CHANNEL_VALUE);
        }

        /**
         * @brief Sets the alpha channel.
         * @param alpha The new alpha value.
         */
        constexpr void setAlpha(const int alpha) noexcept {
            a = static_cast<float>(alpha);
        }

        /**
         * @brief Sets the red channel.
         * @param red The new red value.
         */
        constexpr void setRed(const int red) noexcept {
            r = static_cast<float>(red);
        }

        /**
         * @brief Sets the green channel.
         * @param green The new green value.
         */
        constexpr void setGreen(const int green) noexcept {
            g = static_cast<float>(green);
        }

        /**
         * @brief Sets the blue channel.
         * @param blue The new blue value.
         */
        constexpr void setBlue(const int blue) noexcept {
            b = static_cast<float>(blue);
        }
    };
}
