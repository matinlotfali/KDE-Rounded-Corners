//
// Created by matin on 07/05/24.
//

#ifndef KWIN4_EFFECT_SHAPECORNERS_COLOR_H
#define KWIN4_EFFECT_SHAPECORNERS_COLOR_H

#include <QColor>
#include <sstream>
#include <cmath>

namespace ShapeCorners {
    struct Color {

        float r;
        float g;
        float b;
        float a;

        constexpr Color() :
                Color(0, 0, 0, 0) {};

        explicit Color(const QColor &c) :
                Color(c.red(), c.green(), c.blue(), c.alpha()) {}

        constexpr Color(int r, int g, int b, int a = 255) :
                r(static_cast<float>(r)),
                g(static_cast<float>(g)),
                b(static_cast<float>(b)),
                a(static_cast<float>(a)) {}

        [[maybe_unused]] constexpr Color(float r, float g, float b, float a = 255.0f) :
                r(r), g(g), b(b), a(a) {}

        constexpr Color operator-(const Color &c) const {
            return {
                r - c.r,
                g - c.g,
                b - c.b,
                a - c.a
            };
        }

        constexpr Color operator+(const Color &c) const {
            return {
                r + c.r,
                g + c.g,
                b + c.b,
                a + c.a
            };
        }

        template<typename T>
        constexpr Color operator*(const T &f) const {
            return {
                r * f,
                g * f,
                b * f,
                a * f
            };
        }

        template<typename T>
        constexpr Color operator/(const T &f) const {
            return {
                r / f,
                g / f,
                b / f,
                a / f
            };
        }

        [[nodiscard]] constexpr bool isZero() const {
            return (
                r == 0 &&
                g == 0 &&
                b == 0 &&
                a == 0
            );
        }

        [[nodiscard]] QColor toQColor() const {
            return {
                static_cast<int>(r),
                static_cast<int>(g),
                static_cast<int>(b),
                static_cast<int>(a)
            };
        }

        [[maybe_unused]] [[nodiscard]] std::string toString() const {
            std::stringstream ss;
            ss << "[ " << r << " , " << g << " , " << b << " , " << a << " ]";
            return ss.str();
        }

        constexpr void operator+=(const Color &c) {
            r += c.r;
            g += c.g;
            b += c.b;
            a += c.a;
        }

        constexpr void round() {
            r = std::round(r);
            g = std::round(g);
            b = std::round(b);
            a = std::round(a);
        }

        constexpr void clamp() {
            r = std::clamp(r, 0.0f, 255.0f);
            g = std::clamp(g, 0.0f, 255.0f);
            b = std::clamp(b, 0.0f, 255.0f);
            a = std::clamp(a, 0.0f, 255.0f);
        }

        constexpr void setAlpha(int alpha) {
            a = static_cast<float>(alpha);
        }

        [[maybe_unused]] constexpr void setRed(int red) {
            r = static_cast<float>(red);
        }

        [[maybe_unused]] constexpr void setGreen(int green) {
            g = static_cast<float>(green);
        }

        [[maybe_unused]] constexpr void setBlue(int blue) {
            b = static_cast<float>(blue);
        }
    };
}

#endif //KWIN4_EFFECT_SHAPECORNERS_COLOR_H
