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
#include "Window.h"
#include "TileChecker.h"

#if QT_VERSION_MAJOR >= 6
    #include <effect/offscreeneffect.h>
#else
    #include <kwinoffscreeneffect.h>
#endif

namespace ShapeCorners {
    class Effect final : public KWin::OffscreenEffect {
    Q_OBJECT

    public:
        Effect();

        ~Effect() override;

        static bool supported();

        void reconfigure(ReconfigureFlags flags) override;
        void prePaintWindow(KWin::EffectWindow *w, KWin::WindowPrePaintData &data, std::chrono::milliseconds time) override;

#if QT_VERSION_MAJOR >= 6
        void drawWindow(const KWin::RenderTarget &RenderTarget, const KWin::RenderViewport &viewport,
                        KWin::EffectWindow *w, int mask, const QRegion &region, KWin::WindowPaintData &data) override;
#else
        void drawWindow(KWin::EffectWindow *w, int mask, const QRegion &region, KWin::WindowPaintData &data) override;
#endif

        [[nodiscard]] int requestedEffectChainPosition() const override { return 99; }
        [[nodiscard]] bool blocksDirectScanout() const override { return false; }
        [[nodiscard]] bool isActive() const override { return m_shaderManager.IsValid(); }
        [[nodiscard]] bool provides(Feature feature) override { return feature == Feature::Nothing; }

    public Q_SLOTS:
        [[nodiscard]] QString get_window_titles() const;

    private Q_SLOTS:
        void windowAdded(KWin::EffectWindow *window);
        void windowRemoved(KWin::EffectWindow *window);
        void checkMaximized(KWin::EffectWindow *window);
        void windowResized(KWin::EffectWindow *window, const QRectF &);

    private:
        // Pair of Window pointers and their maximized/tiled state.
        std::unordered_map<const KWin::EffectWindow *, Window> m_managed;
        Shader m_shaderManager;

        void checkTiled();
    };
}
