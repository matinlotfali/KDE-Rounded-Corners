/*
 *   Copyright © 2015 Robert Metsäranta <therealestrob@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 2 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *   General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; see the file COPYING.  if not, write to
 *   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *   Boston, MA 02110-1301, USA.
 */

#pragma once

#include "Shader.h"
#include <QObject>
#if QT_VERSION_MAJOR >= 6
#include <effect/offscreeneffect.h>
#else
#include <kwinoffscreeneffect.h>
#endif

namespace ShapeCorners {
    class WindowManager;

    /**
     * @class Effect
     * @brief KDE KWin effect for rendering rounded corners on windows.
     *
     * This class implements a KWin OffscreenEffect that applies rounded corners
     * to windows using a custom shader. It manages window addition, configuration,
     * and painting logic.
     */
    class Effect final : public KWin::OffscreenEffect {
    Q_OBJECT

    public:
        /**
         * @brief Constructs the Effect object.
         */
        Effect();

        /**
         * @brief Destructor.
         */
        ~Effect() override;

        /**
         * @brief Checks if the effect is supported on the current system.
         * @return True if supported, false otherwise.
         */
        static bool supported();

        /**
         * @brief Reconfigures the effect based on the given flags.
         * @param flags The reconfiguration flags.
         */
        void reconfigure(ReconfigureFlags flags) override;
        
        /**
         * @brief Prepares a window for painting.
         * @param w The effect window.
         * @param data The window pre-paint data.
         * @param time The time since the last frame.
         */
        void prePaintWindow(KWin::EffectWindow *w, KWin::WindowPrePaintData &data, std::chrono::milliseconds time) override;

#if QT_VERSION_MAJOR >= 6
        /**
         * @brief Draws the window with the effect applied (Qt6 version).
         * @param RenderTarget The render target.
         * @param viewport The render viewport.
         * @param w The effect window.
         * @param mask The paint mask.
         * @param region The paint region.
         * @param data The window paint data.
         */
        void drawWindow(const KWin::RenderTarget &RenderTarget, const KWin::RenderViewport &viewport,
                        KWin::EffectWindow *w, int mask, const QRegion &region, KWin::WindowPaintData &data) override;
#else
        /**
         * @brief Draws the window with the effect applied (Qt5 version).
         * @param w The effect window.
         * @param mask The paint mask.
         * @param region The paint region.
         * @param data The window paint data.
         */
        void drawWindow(KWin::EffectWindow *w, int mask, const QRegion &region, KWin::WindowPaintData &data) override;
#endif

        /**
         * @brief Returns the requested position in the effect chain.
         * @return The effect chain position.
         */
        [[nodiscard]] 
        int requestedEffectChainPosition() const override { return 99; }
        
        /**
         * @brief Indicates whether the effect blocks direct scanout.
         * @return False, this effect does not block direct scanout.
         */
        [[nodiscard]]
        bool blocksDirectScanout() const override { return false; }
        
        /**
         * @brief Checks if the effect is currently active.
         * @return True if the shader manager is valid, false otherwise.
         */
        [[nodiscard]]
        bool isActive() const override { return m_shaderManager.IsValid(); }
        
        /**
         * @brief Indicates which features this effect provides.
         * @param feature The feature to check.
         * @return True if the feature is provided, false otherwise.
         */
        [[nodiscard]] 
        bool provides(const Feature feature) override { return feature == Nothing; }

    private Q_SLOTS:
        /**
         * @brief Slot called when a new window is added.
         * @param kwindow The newly added effect window.
         */
        void windowAdded(KWin::EffectWindow *kwindow);

    private:
        /// Manages the shader used for rounded corners.
        Shader m_shaderManager;
        /// Manages the windows affected by the effect.
        std::unique_ptr<WindowManager> m_windowManager;
    };
}
