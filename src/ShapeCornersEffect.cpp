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

#include "ShapeCornersEffect.h"
#include <kwingltexture.h>

#if KWIN_EFFECT_API_VERSION >= 235
#include <KX11Extras>
#else
#include <kwindowsystem.h>
#endif

ShapeCornersEffect::ShapeCornersEffect()
#if KWIN_EFFECT_API_VERSION >= 236
    : KWin::OffscreenEffect()
#else
    : KWin::DeformEffect()
#endif
{
    reconfigure(ReconfigureAll);

    if(m_shaderManager.IsValid()) {
#if KWIN_EFFECT_API_VERSION >= 235
        const auto& windowList = KX11Extras::windows();
#else
        const auto& windowList = KWindowSystem::windows();
#endif
        for (const auto& id: windowList)
            if (auto win = KWin::effects->findWindow(id))
                windowAdded(win);
        connect(KWin::effects, &KWin::EffectsHandler::windowAdded, this, &ShapeCornersEffect::windowAdded);
        connect(KWin::effects, &KWin::EffectsHandler::windowDeleted, this, &ShapeCornersEffect::windowRemoved);
    }
}

ShapeCornersEffect::~ShapeCornersEffect() = default;

void
ShapeCornersEffect::windowAdded(KWin::EffectWindow *w)
{
    if (m_managed.contains(w))
        return;
    qDebug() << w->windowRole() << w->windowType() << w->windowClass();
    if (!w->hasDecoration())
        return;
    m_managed.insert(w);

    redirect(w);
    setShader(w, m_shaderManager.GetShader().get());
}

void ShapeCornersEffect::windowRemoved(KWin::EffectWindow *w)
{
    m_managed.remove(w);
    unredirect(w);
}

void
ShapeCornersEffect::reconfigure(ReconfigureFlags flags)
{
    Q_UNUSED(flags)
    ShapeCornersConfig::self()->read();
}

bool isMaximized(const KWin::EffectWindow *w) {
    if (w == nullptr)
        return false;

    auto screen = KWin::effects->findScreen(w->screen()->name());
    if (screen == nullptr)
        return false;

    auto screenGeometry = screen->geometry();
    return (w->x() == screenGeometry.x() && w->width() == screenGeometry.width()) ||
           (w->y() == screenGeometry.y() && w->height() == screenGeometry.height());
}
QRectF operator *(QRect r, qreal scale) { return {r.x() * scale, r.y() * scale, r.width() * scale, r.height() * scale}; }
QRectF operator *(QRectF r, qreal scale) { return {r.x() * scale, r.y() * scale, r.width() * scale, r.height() * scale}; }
QRect toRect(const QRectF& r) { return {(int)r.x(), (int)r.y(), (int)r.width(), (int)r.height()}; }
const QRect& toRect(const QRect& r) { return r; }

void ShapeCornersEffect::prePaintWindow(KWin::EffectWindow *w, KWin::WindowPrePaintData &data, std::chrono::milliseconds time)
{
    if (!m_shaderManager.IsValid()
            || !m_managed.contains(w)
            || !enabled()
            || isMaximized(w)
        )
    {
        Effect::prePaintWindow(w, data, time);
        return;
    }
    auto size = (int)ShapeCornersConfig::size();

#if KWIN_EFFECT_API_VERSION >= 234
    const auto geo = w->frameGeometry() * KWin::effects->renderTargetScale();
    data.opaque -= QRect(geo.x(), geo.y(), size, size);
    data.opaque -= QRect(geo.x()+geo.width()-size, geo.y(), size, size);
    data.opaque -= QRect(geo.x(), geo.y()+geo.height()-size, size, size);
    data.opaque -= QRect(geo.x()+geo.width()-size, geo.y()+geo.height()-size, size, size);
    data.setTranslucent();
#else
    const auto& geo = w->frameGeometry();
#endif
    data.paint += QRect(geo.x(), geo.y(), size, size);
    data.paint += QRect(geo.x()+geo.width()-size, geo.y(), size, size);
    data.paint += QRect(geo.x(), geo.y()+geo.height()-size, size, size);
    data.paint += QRect(geo.x()+geo.width()-size, geo.y()+geo.height()-size, size, size);

#if KWIN_EFFECT_API_VERSION >= 236
    OffscreenEffect::prePaintWindow(w, data, time);
#else
    DeformEffect::prePaintWindow(w, data, time);
#endif
}

bool ShapeCornersEffect::supported()
{
    return KWin::effects->isOpenGLCompositing();
}

void ShapeCornersEffect::drawWindow(KWin::EffectWindow *w, int mask, const QRegion &region,
                                    KWin::WindowPaintData &data) {
    if (!m_shaderManager.IsValid()
        || !m_managed.contains(w)
        || !enabled()
        || isMaximized(w)
        )
    {
        unredirect(w);
#if KWIN_EFFECT_API_VERSION >= 236
        OffscreenEffect::drawWindow(w, mask, region, data);
#else
        DeformEffect::drawWindow(w, mask, region, data);
#endif
        return;
    }

    redirect(w);
    setShader(w, m_shaderManager.GetShader().get());
    m_shaderManager.Bind(w);
    glActiveTexture(GL_TEXTURE0);

#if KWIN_EFFECT_API_VERSION >= 236
    OffscreenEffect::drawWindow(w, mask, region, data);
#else
    DeformEffect::drawWindow(w, mask, region, data);
#endif
    m_shaderManager.Unbind();
}

bool ShapeCornersEffect::enabled() {
    return !isMaximized(KWin::effects->activeWindow());
}
