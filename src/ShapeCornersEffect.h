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

#include <kwineffects.h>
#include <set>
#include "ShapeCornersShader.h"

#if KWIN_EFFECT_API_VERSION >= 236
#include <kwinoffscreeneffect.h>
class ShapeCornersEffect : public KWin::OffscreenEffect
#else
#include <kwindeformeffect.h>
class ShapeCornersEffect : public KWin::DeformEffect
#endif
{
    Q_OBJECT
public:
    ShapeCornersEffect();
    ~ShapeCornersEffect() override;

    static bool supported();
    static bool enabledByDefault() { return supported(); }
    static bool enabled();

    void reconfigure(ReconfigureFlags flags) override;

    void prePaintWindow(KWin::EffectWindow *w, KWin::WindowPrePaintData &data, std::chrono::milliseconds time) override;
    void drawWindow(KWin::EffectWindow *window, int mask, const QRegion &region, KWin::WindowPaintData &data) override;

    int requestedEffectChainPosition() const override { return 99; }

protected Q_SLOTS:
    void windowAdded(KWin::EffectWindow *window);
    void windowRemoved(KWin::EffectWindow *window);

private:
    std::set<const KWin::EffectWindow*> m_managed;
    ShapeCornersShader m_shaderManager;

    bool hasEffect(const KWin::EffectWindow *w) const;
    static bool isMaximized(const KWin::EffectWindow *w);
};

