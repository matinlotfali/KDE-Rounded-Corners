/**
 * @file Utils.h
 * @brief Utility functions for geometry and vector conversions used in ShapeCorners shaders.
 *
 * This file provides helper functions for scaling QRect/QRectF and converting QSizeF to QVector2D
 */

#pragma once

#include <QRectF>

namespace ShapeCorners {

    /**
     * @brief Scales a QRect by a floating-point scale factor.
     * @param rect The QRect to scale.
     * @param scale The scale factor.
     * @return The scaled QRectF.
     */
    constexpr QRectF operator*(const QRect &rect, const qreal scale) noexcept {
        return {
            rect.x() * scale,
            rect.y() * scale,
            rect.width() * scale,
            rect.height() * scale
        };
    }

    /**
     * @brief Scales a QRectF by a floating-point scale factor.
     * @param rect The QRectF to scale.
     * @param scale The scale factor.
     * @return The scaled QRectF.
     */
    constexpr QRectF operator*(const QRectF &rect, const qreal scale) noexcept {
        return {
            rect.x() * scale,
            rect.y() * scale,
            rect.width() * scale,
            rect.height() * scale
        };
    }
}