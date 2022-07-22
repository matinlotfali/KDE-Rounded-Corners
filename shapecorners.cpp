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

#include "dbus.h"
#include "shapecorners.h"
#include <QPainter>
#include <QImage>
#include <QDBusConnection>
#include <kwindowsystem.h>
#include <kwingltexture.h>


ShapeCornersEffect::ShapeCornersEffect() : KWin::Effect()
{
    new KWin::EffectAdaptor(this);
    QDBusConnection::sessionBus().registerObject("/ShapeCorners", this);

    if(m_shaderManager.IsValid()) {
        for (int i = 0; i < KWindowSystem::windows().count(); ++i)
            if (KWin::EffectWindow *win = KWin::effects->findWindow(KWindowSystem::windows().at(i)))
                windowAdded(win);
        connect(KWin::effects, &KWin::EffectsHandler::windowAdded, this, &ShapeCornersEffect::windowAdded);
        connect(KWin::effects, &KWin::EffectsHandler::windowClosed, this, [this](){ m_managed.removeOne(dynamic_cast<KWin::EffectWindow *>(sender())); });
    }
    else
        deleteLater();

    reconfigure(ReconfigureAll);
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
    m_managed << w;
}

void
ShapeCornersEffect::reconfigure(ReconfigureFlags flags)
{
    Q_UNUSED(flags)
    m_config.Load();
}

#if KWIN_EFFECT_API_VERSION > 231
void
ShapeCornersEffect::prePaintWindow(KWin::EffectWindow *w, KWin::WindowPrePaintData &data, std::chrono::milliseconds time)
#else
void
ShapeCornersEffect::prePaintWindow(KWin::EffectWindow *w, KWin::WindowPrePaintData &data, int time)
#endif
{
    if (!m_shaderManager.IsValid()
            || !m_managed.contains(w)
//            || KWin::effects->hasActiveFullScreenEffect()
            || isMaximized(w)
#if KWIN_EFFECT_API_VERSION < 234
            || !w->isPaintingEnabled()
#elif KWIN_EFFECT_API_VERSION < 233
           || data.quads.isTransformed()
#endif
            )
    {
        KWin::effects->prePaintWindow(w, data, time);
        return;
    }

#if KWIN_EFFECT_API_VERSION <= 235
    const QRect& geo = w->frameGeometry();
#else
    const QRectF& geoF = w->frameGeometry();
    const QRect geo ((int)geo.left(), (int)geo.top(), (int)geo.width(), (int)geo.height());
#endif
    data.paint += geo;
#if KWIN_EFFECT_API_VERSION < 234
    data.clip -= geo;
#endif
    QRegion outerRect (geo.x()+m_config.m_size-1, geo.y()-1,
                       geo.width()-m_config.m_size*2+1, geo.height()-m_config.m_size*2+1, QRegion::Ellipse);
    data.paint += outerRect;
#if KWIN_EFFECT_API_VERSION < 234
    data.clip -=outerRect;
#endif
    KWin::effects->prePaintWindow(w, data, time);
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
ShapeCornersEffect::paintWindow(KWin::EffectWindow *w, int mask, QRegion region, KWin::WindowPaintData &data)
{
    if (!m_shaderManager.IsValid()
            || !m_managed.contains(w)
//            || KWin::effects->hasActiveFullScreenEffect()
            || isMaximized(w)
#if KWIN_EFFECT_API_VERSION < 234
            || !w->isPaintingEnabled()
#elif KWIN_EFFECT_API_VERSION < 233
            || data.quads.isTransformed()
            || !hasShadow(data.quads)
#endif
            || (mask & (PAINT_SCREEN_WITH_TRANSFORMED_WINDOWS))
            )
    {
        KWin::effects->paintWindow(w, mask, region, data);
        return;
    }

#if KWIN_EFFECT_API_VERSION < 233
    const KWin::WindowQuadList qds(data.quads);
    //paint the shadow
    data.quads = qds.select(KWin::WindowQuadShadow);
    KWin::effects->paintWindow(w, mask, region, data);
#endif

    //copy the background
#if KWIN_EFFECT_API_VERSION <= 235
    const QRect& geo = w->frameGeometry();
#else
    const QRectF& geoF = w->frameGeometry();
    const QRect geo ((int)geo.left(), (int)geo.top(), (int)geo.width(), (int)geo.height());
#endif
    const auto& s = KWin::effects->virtualScreenGeometry();
    KWin::GLTexture back (GL_RGBA8, geo.size());
    back.bind();
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, geo.x(), s.height() - geo.y() - geo.height(), geo.width(), geo.height());
    back.unbind();

    //paint the actual window
#if KWIN_EFFECT_API_VERSION < 233
    data.quads = qds.filterOut(KWin::WindowQuadShadow);
#endif
    KWin::effects->paintWindow(w, mask, region, data);

    //'shape' the corners
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    {
        m_shaderManager.Bind(data.screenProjectionMatrix(), geo, KWin::effects->activeWindow() == w, m_config);
        back.bind();
        back.render(region, geo, true);
        back.unbind();
        m_shaderManager.Unbind();
    }
    glDisable(GL_BLEND);
    glDisable(GL_SCISSOR_TEST);

#if KWIN_EFFECT_API_VERSION < 233
    data.quads = qds;
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

bool ShapeCornersEffect::isMaximized(KWin::EffectWindow *w) {
#if KWIN_EFFECT_API_VERSION >= 233
    auto screenGeometry = KWin::effects->findScreen(w->screen()->name())->geometry();
    return (w->x() == screenGeometry.x() && w->width() == screenGeometry.width()) ||
            (w->y() == screenGeometry.y() && w->height() == screenGeometry.height());
#else
    return w->isFullScreen();
#endif
}
