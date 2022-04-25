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
#include <QtDBus/QDBusConnection>

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
        m_shader = KWin::ShaderManager::instance()->generateCustomShader(KWin::ShaderTrait::MapTexture, QByteArray(), frag);
        file.close();
//        qDebug() << frag;
//        qDebug() << "shader valid: " << m_shader->isValid();
        if (m_shader->isValid())
        {
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

ShapeCornersEffect::~ShapeCornersEffect()
{
    delete m_shader;
}

void
ShapeCornersEffect::windowAdded(KWin::EffectWindow *w)
{
    if (m_managed.contains(w)
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
            || !w->isPaintingEnabled()
//            || KWin::effects->hasActiveFullScreenEffect()
            || w->isDesktop()
            || isMaximized(w)
#if KWIN_EFFECT_API_VERSION < 233
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

    //copy the corner regions
    KWin::GLTexture back = KWin::GLTexture(GL_RGBA8, w->size());
    back.bind();
    glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w->x(), 1050 - w->y() - w->height(), w->width(), w->height());
    back.unbind();

    //paint the actual window
#if KWIN_EFFECT_API_VERSION < 233
    data.quads = qds.filterOut(KWin::WindowQuadShadow);
#endif
    KWin::effects->paintWindow(w, mask, region, data);

    //'shape' the corners
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    const int mvpMatrixLocation = m_shader->uniformLocation("modelViewProjectionMatrix");
    const int radiusLocation = m_shader->uniformLocation("radius");
    const int windowSizeLocation = m_shader->uniformLocation("windowSize");
    KWin::ShaderManager *sm = KWin::ShaderManager::instance();
    sm->pushShader(m_shader);
    QMatrix4x4 mvp = data.screenProjectionMatrix();
    mvp.translate(w->x(), w->y());
    m_shader->setUniform(mvpMatrixLocation, mvp);
    m_shader->setUniform(radiusLocation, (float)m_size);
    m_shader->setUniform(windowSizeLocation, QVector2D(w->width(), w->height()));
    glActiveTexture(GL_TEXTURE0);
    back.bind();
    back.render(region, w->rect());
    back.unbind();
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
    return KWin::effects->isOpenGLCompositing() && KWin::GLRenderTarget::supported();
}

bool ShapeCornersEffect::isMaximized(KWin::EffectWindow *w) {
#if KWIN_EFFECT_API_VERSION < 233
    return w->isFullScreen();
#else
    auto screenGeometry = KWin::effects->findScreen(w->screen()->name())->geometry();
    return (w->x() == screenGeometry.x() && w->width() == screenGeometry.width()) ||
            (w->y() == screenGeometry.y() && w->height() == screenGeometry.height());
#endif
}

#include "shapecorners.moc"
