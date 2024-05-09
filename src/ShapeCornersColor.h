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
        r(0),
        g(0),
        b(0),
        a(0) { };
    explicit ShapeCornersColor(const QColor& color) :
        r(static_cast<float>(color.redF())),
        g(static_cast<float>(color.greenF())),
        b(static_cast<float>(color.blueF())),
        a(static_cast<float>(color.alphaF())) { }
    constexpr ShapeCornersColor(int r, int g, int b, int a = 255) :
        r(static_cast<float>(r)/255.0f),
        g(static_cast<float>(g)/255.0f),
        b(static_cast<float>(b)/255.0f),
        a(static_cast<float>(a)/255.0f) { }
    constexpr ShapeCornersColor(float r, float g, float b, float a = 1.0f) :
        r(r),
        g(g),
        b(b),
        a(a) { }

    constexpr ShapeCornersColor operator- (const ShapeCornersColor& c) const {
        return {
            r - c.r,
            g - c.g,
            b - c.b,
            a - c.b
        };
    }
    constexpr ShapeCornersColor operator+ (const ShapeCornersColor& c) const {
        return {
            r + c.r,
            g + c.g,
            b + c.b,
            a + c.b
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
        QColor result = {};
        result.setRedF(r);
        result.setGreenF(g);
        result.setBlueF(b);
        result.setAlphaF(a);
        return result;
    }
    [[nodiscard]] std::string toString() const {
        std::stringstream ss;
        ss << "[ "<< r << " , " << g << " , " << b << " , " << a << " ]";
        return ss.str();
    }
    constexpr void operator+= (const ShapeCornersColor& c) {
        r += c.r;
        g += c.g;
        b += c.g;
        a += c.a;
    }
    constexpr void round(uint8_t decimals) {
        double d = std::pow(10,decimals);
        r = static_cast<float>(std::round(r * d)/ d);
        g = static_cast<float>(std::round(g * d)/ d);
        b = static_cast<float>(std::round(b * d)/ d);
        a = static_cast<float>(std::round(a * d)/ d);
    }
    constexpr void clamp() {
        r = std::clamp(r, 0.0f, 1.0f);
        g = std::clamp(g, 0.0f, 1.0f);
        b = std::clamp(b, 0.0f, 1.0f);
        a = std::clamp(a, 0.0f, 1.0f);
    }
    constexpr void setAlpha(int alpha){
        a = static_cast<float>(alpha)/255.0f;
    }
    constexpr void setRed(int red){
        a = static_cast<float>(red)/255.0f;
    }
    constexpr void setGreen(int green){
        a = static_cast<float>(green)/255.0f;
    }
    constexpr void setBlue(int blue){
        a = static_cast<float>(blue)/255.0f;
    }
};

#endif //KWIN4_EFFECT_SHAPECORNERS_SHAPECORNERSCOLOR_H
