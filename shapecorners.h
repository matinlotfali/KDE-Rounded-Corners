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

class Q_DECL_EXPORT ShapeCornersEffect : public KWin::Effect
{
    Q_OBJECT
public:
    ShapeCornersEffect();
    ~ShapeCornersEffect() override;

    static bool supported();
    static bool enabledByDefault();
    static bool isMaximized(const KWin::EffectWindow& w);

    void setRoundness(int r);

    void reconfigure(ReconfigureFlags flags) override;
    void paintWindow(KWin::EffectWindow* w, int mask, QRegion region, KWin::WindowPaintData& data) override;
    int requestedEffectChainPosition() const override { return 99; }

protected Q_SLOTS:
    void windowAdded(KWin::EffectWindow* window);

private:
    int m_size;
    std::unique_ptr<KWin::GLShader> m_shader;
    QList<const KWin::EffectWindow*> m_managed;
};

#endif //SHAPECORNERS_H

