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

#ifndef SHAPECORNERS_H
#define SHAPECORNERS_H

#include <kwineffects.h>
#include <memory>

namespace KWin { class GLTexture; }

class Q_DECL_EXPORT ShapeCornersEffect : public KWin::Effect
{
    Q_OBJECT
public:
    ShapeCornersEffect();
    ~ShapeCornersEffect() override;

    static bool supported();
    static bool enabledByDefault();
    static bool isMaximized(KWin::EffectWindow *w);

    void setRoundness(int r);

    void reconfigure(ReconfigureFlags flags) override;
#if KWIN_EFFECT_API_VERSION > 231
    void prePaintWindow(KWin::EffectWindow* w, KWin::WindowPrePaintData& data, std::chrono::milliseconds time) override;
#else
    void prePaintWindow(KWin::EffectWindow* w, KWin::WindowPrePaintData& data, int time) override;
#endif
    void paintWindow(KWin::EffectWindow* w, int mask, QRegion region, KWin::WindowPaintData& data) override;
    int requestedEffectChainPosition() const override { return 99; }

protected Q_SLOTS:
    void windowAdded(KWin::EffectWindow *window);

private:
    enum { TopLeft = 0, TopRight, BottomRight, BottomLeft, NTex };
    int m_size;
    bool m_drawShadow;
    QRegion m_updateRegion;
    std::unique_ptr<KWin::GLShader> m_shader;
    QList<KWin::EffectWindow *> m_managed;
};

#endif //SHAPECORNERS_H

