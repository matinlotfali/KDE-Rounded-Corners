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
#include <kwindowsystem.h>
#include <kwingltexture.h>


ShapeCornersEffect::ShapeCornersEffect() : KWin::Effect()
{
    if(m_shaderManager.IsValid()) {
        for (const auto& id: KWindowSystem::windows())
            if (auto win = KWin::effects->findWindow(id))
                windowAdded(win);
        connect(KWin::effects, &KWin::EffectsHandler::windowAdded, this, &ShapeCornersEffect::windowAdded);
        connect(KWin::effects, &KWin::EffectsHandler::windowClosed, this, &ShapeCornersEffect::windowRemoved);
        connect(KWin::effects, &KWin::EffectsHandler::windowActivated, this,&ShapeCornersEffect::windowGetBackground);
        connect(KWin::effects, &KWin::EffectsHandler::windowFrameGeometryChanged, this,&ShapeCornersEffect::windowGetBackground);
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
            || w->windowType() == NET::WindowType::OnScreenDisplay
            || w->windowType() == NET::WindowType::Dock)
        return;
//    qDebug() << w->windowRole() << w->windowType() << w->windowClass();
    if (!w->hasDecoration() && (w->windowClass().contains("plasma", Qt::CaseInsensitive)
            || w->windowClass().contains("krunner", Qt::CaseInsensitive)
            || w->windowClass().contains("latte-dock", Qt::CaseInsensitive)))
        return;
    m_managed.insert(w, nullptr);
}

void ShapeCornersEffect::windowRemoved(KWin::EffectWindow *w)
{
    m_managed.remove(w);
}

void
ShapeCornersEffect::reconfigure(ReconfigureFlags flags)
{
    Q_UNUSED(flags)
    m_config.Load();
}

#if KWIN_EFFECT_API_VERSION < 233
static bool hasShadow(KWin::WindowQuadList &qds)
{
    for (int i = 0; i < qds.count(); ++i)
        if (qds.at(i).type() == KWin::WindowQuadShadow)
            return true;
    return false;
}
#endif

void
ShapeCornersEffect::drawWindow(KWin::EffectWindow *w, int mask, const QRegion& region, KWin::WindowPaintData &data)
{
    if (!m_shaderManager.IsValid()
            || !m_managed.contains(w)
            || isMaximized(w)
#if KWIN_EFFECT_API_VERSION < 234
            || !w->isPaintingEnabled()
#elif KWIN_EFFECT_API_VERSION < 233
            || data.quads.isTransformed()
            || !hasShadow(data.quads)
#endif
            )
    {
        KWin::effects->drawWindow(w, mask, region, data);
        return;
    }

    //copy the background
    if(!m_managed[w]) {
#if KWIN_EFFECT_API_VERSION < 235
        const QRect &geo = w->frameGeometry();
#else
        const QRectF& geoF = w->frameGeometry();
        const QRect geo ((int)geo.left(), (int)geo.top(), (int)geo.width(), (int)geo.height());
#endif
        const auto &s = KWin::effects->virtualScreenGeometry();
        m_managed[w].reset(new KWin::GLTexture(GL_RGBA8, geo.size()));
        m_managed[w]->bind();
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                            geo.x(), s.height() - geo.y() - geo.height(),
                            geo.width(), geo.height());
        m_managed[w]->unbind();
    }

    //'shape' the corners
    auto &shader = m_shaderManager.Bind(isWindowActive(w), w->hasDecoration(), m_config);
    data.shader = shader.get();
    glActiveTexture(GL_TEXTURE1);
    m_managed[w]->bind();
    glActiveTexture(GL_TEXTURE0);
    KWin::effects->drawWindow(w, mask, region, data);
    m_managed[w]->unbind();
    m_shaderManager.Unbind();
}

bool ShapeCornersEffect::supported()
{
#if KWIN_EFFECT_API_VERSION < 234
    return KWin::effects->isOpenGLCompositing() && KWin::GLRenderTarget::supported();
#else
    return KWin::effects->isOpenGLCompositing();
#endif
}

bool ShapeCornersEffect::isMaximized(KWin::EffectWindow *w) {
#if KWIN_EFFECT_API_VERSION >= 233
    auto screenGeometry = KWin::effects->findScreen(w->screen()->name())->geometry();
    return (w->x() == screenGeometry.x() && w->width() == screenGeometry.width()) ||
            (w->y() == screenGeometry.y() && w->height() == screenGeometry.height());
#else
    return w->isFullScreen();
#endif
}

bool ShapeCornersEffect::isWindowActive(KWin::EffectWindow *w) {
    return KWin::effects->activeWindow() == w;
}

void ShapeCornersEffect::windowGetBackground(KWin::EffectWindow *window) {
    if(m_managed.contains(window))
        m_managed[window].reset();
}
