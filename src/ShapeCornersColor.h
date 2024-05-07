//
// Created by matin on 07/05/24.
//

#ifndef KWIN4_EFFECT_SHAPECORNERS_SHAPECORNERSCOLOR_H
#define KWIN4_EFFECT_SHAPECORNERS_SHAPECORNERSCOLOR_H

#include <QColor>

struct ShapeCornersColor : public QColor {
    constexpr ShapeCornersColor() = default;
    constexpr ShapeCornersColor(const QColor& color): QColor(color) { }
    constexpr ShapeCornersColor(int r, int g, int b, int a = 255) : QColor(r, g, b, a) { }
    constexpr ShapeCornersColor(float r, float g, float b, float a = 1.0f) {
        setRedF(r);
        setGreenF(g);
        setBlueF(b);
        setAlphaF(a);
    }

    ShapeCornersColor operator- (const QColor& a) const {
        return {
                red() - a.red(),
                green() - a.green(),
                blue() - a.blue(),
                alpha() - a.alpha()
        };
    }
    ShapeCornersColor operator+ (const QColor& a) const {
        return {
                red() + a.red(),
                green() + a.green(),
                blue() + a.blue(),
                alpha() + a.alpha()
        };
    }
    ShapeCornersColor operator/ (const float& f) const {
        return {
                redF() / f,
                greenF() / f,
                blueF() / f,
                alphaF() / f
        };
    }
    void operator+= (const QColor& a) {
        setRed(red() + a.red());
        setGreen(green() + a.green());
        setBlue(blue() + a.blue());
        setAlpha(alpha() + a.alpha());
    }

    [[nodiscard]] bool isZero() const { return rgba()==0; }
};

#endif //KWIN4_EFFECT_SHAPECORNERS_SHAPECORNERSCOLOR_H
