//
// Created by matin on 07/05/24.
//

#ifndef KWIN4_EFFECT_SHAPECORNERS_SHAPECORNERSCOLOR_H
#define KWIN4_EFFECT_SHAPECORNERS_SHAPECORNERSCOLOR_H

#include <QColor>
#include <sstream>
#include <cmath>

struct ShapeCornersColor {

    float r;
    float g;
    float b;
    float a;

    constexpr ShapeCornersColor() :
        ShapeCornersColor(0,0,0,0) { };
    explicit ShapeCornersColor(const QColor& color) :
        ShapeCornersColor(color.red(), color.green(), color.blue(), color.alpha()) { }
    constexpr ShapeCornersColor(int r, int g, int b, int a = 255) :
        r(static_cast<float>(r)),
        g(static_cast<float>(g)),
        b(static_cast<float>(b)),
        a(static_cast<float>(a)) { }
    constexpr ShapeCornersColor(float r, float g, float b, float a = 255.0f) :
        r(r), g(g), b(b), a(a) { }

    constexpr ShapeCornersColor operator- (const ShapeCornersColor& c) const {
        return {
            r - c.r,
            g - c.g,
            b - c.b,
            a - c.a
        };
    }
    constexpr ShapeCornersColor operator+ (const ShapeCornersColor& c) const {
        return {
            r + c.r,
            g + c.g,
            b + c.b,
            a + c.a
        };
    }
    template<typename T>
    constexpr ShapeCornersColor operator* (const T& f) const {
        return {
            r * f,
            g * f,
            b * f,
            a * f
        };
    }
    template<typename T>
    constexpr ShapeCornersColor operator/ (const T& f) const {
        return {
            r / f,
            g / f,
            b / f,
            a / f
        };
    }
    [[nodiscard]] constexpr bool isZero() const {
        return (
            r==0 &&
            g==0 &&
            b==0 &&
            a==0
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
    [[nodiscard]] std::string toString() const {
        std::stringstream ss;
        ss << "[ "<< r << " , " << g << " , " << b << " , " << a << " ]";
        return ss.str();
    }
    constexpr void operator+= (const ShapeCornersColor& c) {
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
    constexpr void setAlpha(int alpha){
        a = static_cast<float>(alpha);
    }
    constexpr void setRed(int red){
        r = static_cast<float>(red);
    }
    constexpr void setGreen(int green){
        g = static_cast<float>(green);
    }
    constexpr void setBlue(int blue){
        b = static_cast<float>(blue);
    }
};

#endif //KWIN4_EFFECT_SHAPECORNERS_SHAPECORNERSCOLOR_H
