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
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <kwinglplatform.h>
#include <kwinglutils.h>
#include <kwindowsystem.h>
#include <QMatrix4x4>
#include <KConfigGroup>
#include <QDBusConnection>

#if KWIN_EFFECT_API_VERSION < 233
KWIN_EFFECT_FACTORY_SUPPORTED_ENABLED(  ShapeCornersFactory,
                                        ShapeCornersEffect,
                                        "shapecorners.json",
                                        return ShapeCornersEffect::supported();,
                                        return ShapeCornersEffect::enabledByDefault();)
#else
KWIN_EFFECT_FACTORY_SUPPORTED_ENABLED(  ShapeCornersEffect,
                                        "shapecorners.json",
                                        return ShapeCornersEffect::supported();,
                                        return ShapeCornersEffect::enabledByDefault();)
#endif


ShapeCornersEffect::ShapeCornersEffect() : KWin::Effect(), m_shader(nullptr)
{
    new KWin::EffectAdaptor(this);
    QDBusConnection::sessionBus().registerObject("/ShapeCorners", this);
    reconfigure(ReconfigureAll);

    QString shadersDir(QStringLiteral("kwin/shaders/1.10/"));
#ifdef KWIN_HAVE_OPENGLES
    const qint64 coreVersionNumber = kVersionNumber(3, 0);
#else
    const qint64 version = KWin::kVersionNumber(1, 40);
#endif
    if (KWin::GLPlatform::instance()->glslVersion() >= version)
        shadersDir = QStringLiteral("kwin/shaders/1.40/");

    const QString fragmentshader = QStandardPaths::locate(QStandardPaths::GenericDataLocation, shadersDir + QStringLiteral("shapecorners.frag"));
//    m_shader = KWin::ShaderManager::instance()->loadFragmentShader(KWin::ShaderManager::GenericShader, fragmentshader);
    QFile file(fragmentshader);
    if (file.open(QFile::ReadOnly))
    {
        QByteArray frag = file.readAll();
        auto shader = KWin::ShaderManager::instance()->generateCustomShader(KWin::ShaderTrait::MapTexture, QByteArray(), frag);
#if KWIN_EFFECT_API_VERSION >= 235
        m_shader = std::move(shader);
#else
        m_shader.reset(shader);
#endif
        file.close();
//        qDebug() << frag;
//        qDebug() << "shader valid: " << m_shader->isValid();
        if (m_shader->isValid())
        {
            const int sampler = m_shader->uniformLocation("sampler");
            const int corner = m_shader->uniformLocation("corner");
            KWin::ShaderManager::instance()->pushShader(m_shader.get());
            m_shader->setUniform(corner, 1);
            m_shader->setUniform(sampler, 0);
            KWin::ShaderManager::instance()->popShader();
            for (int i = 0; i < KWindowSystem::windows().count(); ++i)
                if (KWin::EffectWindow *win = KWin::effects->findWindow(KWindowSystem::windows().at(i)))
                    windowAdded(win);
            connect(KWin::effects, &KWin::EffectsHandler::windowAdded, this, &ShapeCornersEffect::windowAdded);
            connect(KWin::effects, &KWin::EffectsHandler::windowClosed, this, [this](){m_managed.removeOne(dynamic_cast<KWin::EffectWindow *>(sender()));});
        }
        else
            qDebug() << "ShapeCorners: no valid shaders found! ShapeCorners will not work.";
    }
    else
    {
        qDebug() << "ShapeCorners: no shaders found! Exiting...";
        deleteLater();
    }
}

ShapeCornersEffect::~ShapeCornersEffect() = default;

void
ShapeCornersEffect::windowAdded(KWin::EffectWindow *w)
{
    if (m_managed.contains(w)
            || w->windowType() == NET::WindowType::OnScreenDisplay
            || w->windowType() == NET::WindowType::Dock)
        return;
//    qDebug() << w->windowRole() << w->windowType() << w->windowClass();
//    if (!w->hasDecoration() && (w->windowClass().contains("plasma", Qt::CaseInsensitive)
//            || w->windowClass().contains("krunner", Qt::CaseInsensitive)
//            || w->windowClass().contains("latte-dock", Qt::CaseInsensitive)))
//        return;
    if (!w->hasDecoration())
        return;
    m_managed << w;
}

void
ShapeCornersEffect::setRoundness(const int r)
{
    m_size = r;
}

void
ShapeCornersEffect::reconfigure(ReconfigureFlags flags)
{
    Q_UNUSED(flags)
    KConfigGroup conf = KSharedConfig::openConfig("shapecorners.conf")->group("General");
    setRoundness(conf.readEntry("roundness", 5));
    m_drawShadow = conf.readEntry("drawShadow", true);
}

#if KWIN_EFFECT_API_VERSION > 231
void
ShapeCornersEffect::prePaintWindow(KWin::EffectWindow *w, KWin::WindowPrePaintData &data, std::chrono::milliseconds time)
#else
void
ShapeCornersEffect::prePaintWindow(KWin::EffectWindow *w, KWin::WindowPrePaintData &data, int time)
#endif
{
    if (!m_shader->isValid()
            || !m_managed.contains(w)
//            || KWin::effects->hasActiveFullScreenEffect()
            || w->isDesktop()
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
    const auto& geo = w->frameGeometry();
    const QRect rect[NTex] =
    {
        QRect(geo.topLeft().x(), geo.topLeft().y(), m_size, m_size),
        QRect(geo.topRight().x()-m_size+1, geo.topRight().y(), m_size, m_size),
        QRect(geo.bottomRight().x()-m_size+1, geo.bottomRight().y()-m_size+1, m_size, m_size),
        QRect(geo.bottomLeft().x(), geo.bottomLeft().y()-m_size+1, m_size, m_size)
    };
    for (int i = 0; i < NTex; ++i)
    {
        data.paint += rect[i];
#if KWIN_EFFECT_API_VERSION < 234
        data.clip -= rect[i];
#endif
    }
    QRegion outerRect (geo.x()+m_size-1, geo.y()-1,
                       geo.width()-m_size*2+1, geo.height()-m_size*2+1, QRegion::Ellipse);
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
    if (!m_shader->isValid()
            || !m_managed.contains(w)
//            || KWin::effects->hasActiveFullScreenEffect()
            || w->isDesktop()
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

    //map the corners
    const auto& geo = w->frameGeometry();
    const QRect rect[NTex] =
    {
        QRect(geo.topLeft().x(), geo.topLeft().y(), m_size, m_size),
        QRect(geo.topRight().x()-m_size+1, geo.topRight().y(), m_size, m_size),
        QRect(geo.bottomRight().x()-m_size+1, geo.bottomRight().y()-m_size+1, m_size, m_size),
        QRect(geo.bottomLeft().x(), geo.bottomLeft().y()-m_size+1, m_size, m_size)
    };

#if KWIN_EFFECT_API_VERSION < 233
    const KWin::WindowQuadList qds(data.quads);
    //paint the shadow
    data.quads = qds.select(KWin::WindowQuadShadow);
    KWin::effects->paintWindow(w, mask, region, data);
#endif

    //copy the corner regions
    QList<KWin::GLTexture> tex;
    const QRect s(KWin::effects->virtualScreenGeometry());
    for (int i = 0; i < NTex; ++i)
    {
        KWin::GLTexture t = KWin::GLTexture(GL_RGBA8, rect[i].size());
        t.bind();
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, rect[i].x(), s.height() - rect[i].y() - rect[i].height(), rect[i].width(), rect[i].height());
        t.unbind();
        tex.append(t);
    }

    //paint the actual window
#if KWIN_EFFECT_API_VERSION < 233
    data.quads = qds.filterOut(KWin::WindowQuadShadow);
#endif
    KWin::effects->paintWindow(w, mask, region, data);

    //'shape' the corners
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    const int mvpMatrixLocation = m_shader->uniformLocation("modelViewProjectionMatrix");
    const int cornerIndexLocation = m_shader->uniformLocation("cornerIndex");
    const int windowActiveLocation = m_shader->uniformLocation("windowActive");
    const int drawShadowLocation = m_shader->uniformLocation("drawShadow");
    KWin::ShaderManager *sm = KWin::ShaderManager::instance();
    sm->pushShader(m_shader.get());
    {
        m_shader->setUniform(windowActiveLocation, KWin::effects->activeWindow() == w);
        m_shader->setUniform(drawShadowLocation, m_drawShadow);
        for (int i = 0; i < NTex; ++i) {
            QMatrix4x4 mvp = data.screenProjectionMatrix();
            mvp.translate(rect[i].x(), rect[i].y());
            m_shader->setUniform(mvpMatrixLocation, mvp);
            m_shader->setUniform(cornerIndexLocation, i);
            tex[i].bind();
            tex[i].render(region, rect[i]);
            tex[i].unbind();
        }
    }
    sm->popShader();
#if KWIN_EFFECT_API_VERSION < 233
    data.quads = qds;
#endif
    glDisable(GL_BLEND);
}

bool
ShapeCornersEffect::enabledByDefault()
{
    return supported();
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

#include "shapecorners.moc"
