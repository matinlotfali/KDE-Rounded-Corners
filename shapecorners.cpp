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

#include "shapecorners.h"
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
    m_config.Load();
}

ShapeCornersEffect::~ShapeCornersEffect() = default;

void
ShapeCornersEffect::windowAdded(KWin::EffectWindow *w)
{
    if (m_managed.contains(w)
            || w->isDesktop()
            || w->isPopupMenu()
            || w->isDropdownMenu()
            || w->isTooltip()
            || w->isMenu()
            || w->isOnScreenDisplay()
            || w->isDock())
        return;
    qDebug() << w->windowRole() << w->windowType() << w->windowClass();
    if (!w->hasDecoration()
            && (w->windowClass().contains("plasma", Qt::CaseInsensitive)
            || w->windowClass().contains("krunner", Qt::CaseInsensitive)
            || w->windowClass().contains("latte-dock", Qt::CaseInsensitive)
            || w->windowClass().contains("telegram", Qt::CaseInsensitive)))
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
    m_config.Load();
}

bool isMaximized(KWin::EffectWindow *w) {
    auto screenGeometry = KWin::effects->findScreen(w->screen()->name())->geometry();
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
            || isMaximized(w)
        )
    {
        Effect::prePaintWindow(w, data, time);
        return;
    }

#if KWIN_EFFECT_API_VERSION >= 234
    const auto geo = w->frameGeometry() * KWin::effects->renderTargetScale();
    data.opaque -= QRect(geo.x(), geo.y(), m_config.m_size, m_config.m_size);
    data.opaque -= QRect(geo.x()+geo.width()-m_config.m_size, geo.y(), m_config.m_size, m_config.m_size);
    data.opaque -= QRect(geo.x(), geo.y()+geo.height()-m_config.m_size, m_config.m_size, m_config.m_size);
    data.opaque -= QRect(geo.x()+geo.width()-m_config.m_size, geo.y()+geo.height()-m_config.m_size, m_config.m_size, m_config.m_size);
    data.setTranslucent();
#else
    const auto& geo = w->frameGeometry();
#endif
    data.paint += QRect(geo.x(), geo.y(), m_config.m_size, m_config.m_size);
    data.paint += QRect(geo.x()+geo.width()-m_config.m_size, geo.y(), m_config.m_size, m_config.m_size);
    data.paint += QRect(geo.x(), geo.y()+geo.height()-m_config.m_size, m_config.m_size, m_config.m_size);
    data.paint += QRect(geo.x()+geo.width()-m_config.m_size, geo.y()+geo.height()-m_config.m_size, m_config.m_size, m_config.m_size);

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
    m_shaderManager.Bind(w, m_config);
    glActiveTexture(GL_TEXTURE0);

#if KWIN_EFFECT_API_VERSION >= 236
    OffscreenEffect::drawWindow(w, mask, region, data);
#else
    DeformEffect::drawWindow(w, mask, region, data);
#endif
    m_shaderManager.Unbind();
}
