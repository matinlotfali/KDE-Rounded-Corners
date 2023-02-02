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

#define DEBUG_BACKGROUND 0

ShapeCornersEffect::ShapeCornersEffect() : KWin::Effect()
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
    if (m_managed.contains(w))
        return;
    qDebug() << w->windowRole() << w->windowType() << w->windowClass();
    if (!w->hasDecoration())
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

bool isMaximized(KWin::EffectWindow *w) {
#if KWIN_EFFECT_API_VERSION >= 233
    auto screenGeometry = KWin::effects->findScreen(w->screen()->name())->geometry();
    return (w->x() == screenGeometry.x() && w->width() == screenGeometry.width()) ||
           (w->y() == screenGeometry.y() && w->height() == screenGeometry.height());
#else
    return w->isFullScreen();
#endif
}
QRectF operator *(QRect r, qreal scale) { return {r.x() * scale, r.y() * scale, r.width() * scale, r.height() * scale}; }
QRectF operator *(QRectF r, qreal scale) { return {r.x() * scale, r.y() * scale, r.width() * scale, r.height() * scale}; }
QRect toRect(const QRectF& r) { return {(int)r.x(), (int)r.y(), (int)r.width(), (int)r.height()}; }
const QRect& toRect(const QRect& r) { return r; }

#if KWIN_EFFECT_API_VERSION >= 233
void ShapeCornersEffect::prePaintWindow(KWin::EffectWindow *w, KWin::WindowPrePaintData &data, std::chrono::milliseconds time)
#else
void ShapeCornersEffect::prePaintWindow(KWin::EffectWindow *w, KWin::WindowPrePaintData &data, int time)
#endif
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
        KWin::effects->prePaintWindow(w, data, time);
        return;
    }

    const auto geo = w->frameGeometry();
#if KWIN_EFFECT_API_VERSION >= 234
    data.opaque -= toRect(geo);
#endif
    data.paint += toRect(geo);
    KWin::effects->prePaintWindow(w, data, time);
}

void
ShapeCornersEffect::paintWindow(KWin::EffectWindow *w, int mask, QRegion region, KWin::WindowPaintData &data)
{
    if (!m_shaderManager.IsValid()
            || !m_managed.contains(w)
            || isMaximized(w)
            || mask & PAINT_WINDOW_TRANSFORMED
#if KWIN_EFFECT_API_VERSION < 234
            || !w->isPaintingEnabled()
#elif KWIN_EFFECT_API_VERSION < 233
            || data.quads.isTransformed()
            || !hasShadow(data.quads)
#endif
            )
    {
        KWin::effects->paintWindow(w, mask, region, data);
        return;
    }

    const bool clipping = region != KWin::infiniteRegion();
#if KWIN_EFFECT_API_VERSION >= 234
    const qreal scale = KWin::effects->renderTargetScale();
    const QRectF renderRect = KWin::effects->renderTargetRect() * scale;
    const QRegion clipRegion = clipping ? KWin::effects->mapToRenderTarget(region) : KWin::infiniteRegion();
#else
    const qreal scale = 1;
    const QRectF renderRect = {0, 0, 0, 0};
    const QRegion clipRegion = region;
#endif
    const QRectF geo = w->frameGeometry() * scale;

    //copy the background
    if(!m_managed[w]) {
        m_managed[w].reset(new KWin::GLTexture(GL_RGBA8, geo.width(), geo.height()));
        m_managed[w]->bind();
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0,
                            geo.x() - renderRect.x(),
                            renderRect.height() - (geo.y() - renderRect.y() + geo.height()),
                            geo.width(), geo.height());
        m_managed[w]->unbind();
    }

    KWin::effects->paintWindow(w, mask, region, data);

    //'shape' the corners
    if (clipping) {
        glEnable(GL_SCISSOR_TEST);
    }
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    {
        QMatrix4x4 mvp;
        mvp.ortho(0, renderRect.width(), renderRect.height(), 0, 0, 65535);
        mvp.translate(geo.x()-renderRect.x(), geo.y()-renderRect.y());
        m_shaderManager.Bind(mvp, w, m_config);
        m_managed[w]->bind();
        m_managed[w]->render(clipRegion, toRect(geo), true);
        m_managed[w]->unbind();
        m_shaderManager.Unbind();
    }
    glDisable(GL_BLEND);
    if (clipping) {
        glDisable(GL_SCISSOR_TEST);
    }

#if DEBUG_BACKGROUND == 1
    if (KWin::effects->activeWindow() == w) {
        auto s = KWin::ShaderManager::instance()->pushShader(KWin::ShaderTrait::MapTexture);
        const QRectF screenRect = w->screen()->geometry() * scale;
        QMatrix4x4 mvp;
        mvp.ortho(0, renderRect.width(), renderRect.height(), 0, 0, 65535);
        mvp.translate(screenRect.x()-renderRect.x(), screenRect.y()-renderRect.y());
        s->setUniform(KWin::GLShader::ModelViewProjectionMatrix, mvp);
        m_managed[w]->bind();
        m_managed[w]->render(region, toRect(QRect(0,0,500,500)*scale));
        m_managed[w]->unbind();
        KWin::ShaderManager::instance()->popShader();
    }
#endif
}

bool ShapeCornersEffect::supported()
{
#if KWIN_EFFECT_API_VERSION < 234
    return KWin::effects->isOpenGLCompositing() && KWin::GLRenderTarget::supported();
#else
    return KWin::effects->isOpenGLCompositing();
#endif
}

void ShapeCornersEffect::windowGetBackground(KWin::EffectWindow *window) {
    if(m_managed.contains(window))
        m_managed[window].reset();
}
