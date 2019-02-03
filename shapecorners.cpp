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
#include <QPainter>
#include <QImage>
#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <kwinglplatform.h>
#include <kwinglutils.h>
#include <QMatrix4x4>

KWIN_EFFECT_FACTORY_SUPPORTED_ENABLED(  ShapeCornersFactory,
                                        ShapeCornersEffect,
                                        "shapecorners.json",
                                        return ShapeCornersEffect::supported();,
                                        return ShapeCornersEffect::enabledByDefault();)


ShapeCornersEffect::ShapeCornersEffect() : KWin::Effect(), m_shader(0)
{
    for (int i = 0; i < NTex; ++i)
    {
        m_tex[i] = 0;
        m_rect[i] = 0;
    }
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
            const int sampler = m_shader->uniformLocation("sampler");
            const int corner = m_shader->uniformLocation("corner");
            KWin::ShaderManager::instance()->pushShader(m_shader);
            m_shader->setUniform(corner, 1);
            m_shader->setUniform(sampler, 0);
            KWin::ShaderManager::instance()->popShader();
        }
    }
    else
        deleteLater();
}

ShapeCornersEffect::~ShapeCornersEffect()
{
    if (m_shader)
        delete m_shader;
    for (int i = 0; i < NTex; ++i)
    {
        if (m_tex[i])
            delete m_tex[i];
        if (m_rect[i])
            delete m_rect[i];
    }
}

void
ShapeCornersEffect::genMasks()
{
    for (int i = 0; i < NTex; ++i)
        if (m_tex[i])
            delete m_tex[i];

    QImage img(m_size*2, m_size*2, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);
    QPainter p(&img);
    p.fillRect(img.rect(), Qt::black);
    p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::black);
    p.setRenderHint(QPainter::Antialiasing);
    p.drawEllipse(img.rect());
    p.end();

    m_tex[TopLeft] = new KWin::GLTexture(img.copy(0, 0, m_size, m_size));
    m_tex[TopRight] = new KWin::GLTexture(img.copy(m_size, 0, m_size, m_size));
    m_tex[BottomRight] = new KWin::GLTexture(img.copy(m_size, m_size, m_size, m_size));
    m_tex[BottomLeft] = new KWin::GLTexture(img.copy(0, m_size, m_size, m_size));
}

void
ShapeCornersEffect::genRect()
{
    for (int i = 0; i < NTex; ++i)
        if (m_rect[i])
            delete m_rect[i];

    m_rSize = m_size+1;
    QImage img(m_rSize*2, m_rSize*2, QImage::Format_ARGB32_Premultiplied);
    img.fill(Qt::transparent);
    QPainter p(&img);
    QRect r(img.rect());
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0, 0, 0, m_alpha));
    p.setRenderHint(QPainter::Antialiasing);
    p.drawEllipse(r);
    p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    p.setBrush(Qt::black);
    r.adjust(1, 1, -1, -1);
    p.drawEllipse(r);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);
    p.setBrush(QColor(255, 255, 255, 63));
    p.drawEllipse(r);
    p.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    p.setBrush(Qt::black);
    r.adjust(0, 1, 0, 0);
    p.drawEllipse(r);
    p.end();

    m_rect[TopLeft] = new KWin::GLTexture(img.copy(0, 0, m_rSize, m_rSize));
    m_rect[TopRight] = new KWin::GLTexture(img.copy(m_rSize, 0, m_rSize, m_rSize));
    m_rect[BottomRight] = new KWin::GLTexture(img.copy(m_rSize, m_rSize, m_rSize, m_rSize));
    m_rect[BottomLeft] = new KWin::GLTexture(img.copy(0, m_rSize, m_rSize, m_rSize));
}

void
ShapeCornersEffect::setRoundness(const int r)
{
    m_alpha = 0xff;
    m_size = r;
    m_corner = QSize(m_size, m_size);
    genMasks();
    genRect();
}

void
ShapeCornersEffect::reconfigure(ReconfigureFlags flags)
{
    Q_UNUSED(flags)
    m_alpha = 63;
    setRoundness(5);
}

void
ShapeCornersEffect::prePaintWindow(KWin::EffectWindow *w, KWin::WindowPrePaintData &data, int time)
{

    if (!m_shader->isValid()
            || (w->windowClass().contains("plasma", Qt::CaseInsensitive) 
                && !w->isNormalWindow() 
                && !w->isDialog()
                && !w->isModal())
            || w->windowClass().contains("krunner", Qt::CaseInsensitive)
            || !w->isPaintingEnabled()
//            || !w->hasDecoration()
            || KWin::effects->activeFullScreenEffect()
            || w->isDesktop()
            || w->isPopupMenu()
//            || w->isModal()
            || data.quads.isTransformed()
            )
    {
        KWin::effects->prePaintWindow(w, data, time);
        return;
    }
    const QRect geo(w->geometry());
    const QRect rect[NTex] =
    {
        QRect(geo.topLeft(), m_corner),
        QRect(geo.topRight()-QPoint(m_size-1, 0), m_corner),
        QRect(geo.bottomRight()-QPoint(m_size-1, m_size-1), m_corner),
        QRect(geo.bottomLeft()-QPoint(0, m_size-1), m_corner)
    };
    for (int i = 0; i < NTex; ++i)
    {
        data.paint += rect[i];
        data.clip -= rect[i];
    }
    QRegion outerRect(QRegion(geo.adjusted(-1, -1, 1, 1))-geo);
    outerRect += QRegion(geo.x()+m_size, geo.y(), geo.width()-m_size*2, 1);
    data.paint += outerRect;
    data.clip -=outerRect;
    KWin::effects->prePaintWindow(w, data, time);
}

static bool hasShadow(KWin::WindowQuadList &qds)
{
    for (int i = 0; i < qds.count(); ++i)
        if (qds.at(i).type() == KWin::WindowQuadShadow)
            return true;
    return false;
}

void
ShapeCornersEffect::paintWindow(KWin::EffectWindow *w, int mask, QRegion region, KWin::WindowPaintData &data)
{
    if (!m_shader->isValid()
            || (w->windowClass().contains("plasma", Qt::CaseInsensitive) 
                && !w->isNormalWindow() 
                && !w->isDialog()
                && !w->isModal())
            || w->windowClass().contains("krunner", Qt::CaseInsensitive)
            || !w->isPaintingEnabled()
//            || !w->hasDecoration()
            || KWin::effects->activeFullScreenEffect()
            || w->isDesktop()
            || w->isPopupMenu()
//            || w->isModal()
            || data.quads.isTransformed()
            || (mask & PAINT_WINDOW_TRANSFORMED)
//            || !hasShadow(data.quads)
            )
    {
        KWin::effects->paintWindow(w, mask, region, data);
        return;
    }

    //map the corners
    const QRect geo(w->geometry());
    const QRect rect[NTex] =
    {
        QRect(geo.topLeft(), m_corner),
        QRect(geo.topRight()-QPoint(m_size-1, 0), m_corner),
        QRect(geo.bottomRight()-QPoint(m_size-1, m_size-1), m_corner),
        QRect(geo.bottomLeft()-QPoint(0, m_size-1), m_corner)
    };

    const KWin::WindowQuadList qds(data.quads);
    //paint the shadow
    data.quads = qds.select(KWin::WindowQuadShadow);
    KWin::effects->paintWindow(w, mask, region, data);

    //copy the corner regions
    KWin::GLTexture tex[NTex];
    const QRect s(KWin::effects->virtualScreenGeometry());
    for (int i = 0; i < NTex; ++i)
    {
        tex[i] = KWin::GLTexture(GL_RGBA8, rect[i].size());
        tex[i].bind();
        glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, rect[i].x(), s.height() - rect[i].y() - rect[i].height(), rect[i].width(), rect[i].height());
        tex[i].unbind();
    }

    //paint the actual window
    data.quads = qds.filterOut(KWin::WindowQuadShadow);
    KWin::effects->paintWindow(w, mask, region, data);

    //'shape' the corners
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    const int mvpMatrixLocation = m_shader->uniformLocation("modelViewProjectionMatrix");
    KWin::ShaderManager *sm = KWin::ShaderManager::instance();
    sm->pushShader(m_shader/*KWin::ShaderTrait::MapTexture*/);

    for (int i = 0; i < NTex; ++i)
    {
        QMatrix4x4 modelViewProjection;
        modelViewProjection.ortho(0, s.width(), s.height(), 0, 0, 65535);
        modelViewProjection.translate(rect[i].x(), rect[i].y());
        m_shader->setUniform(mvpMatrixLocation, modelViewProjection);
        glActiveTexture(GL_TEXTURE1);
        m_tex[3-i]->bind();
        glActiveTexture(GL_TEXTURE0);
        tex[i].bind();

        tex[i].render(region, rect[i]);

        tex[i].unbind();
        m_tex[3-i]->unbind();
    }
    sm->popShader();
    data.quads = qds;
#if 0
    if (data.brightness() == 1.0 && data.crossFadeProgress() == 1.0)
    {
        const QRect rrect[NTex] =
        {
            rect[0].adjusted(-1, -1, 0, 0),
            rect[1].adjusted(0, -1, 1, 0),
            rect[2].adjusted(0, 0, 1, 1),
            rect[3].adjusted(-1, 0, 0, 1)
        };
        const float o(data.opacity());
        KWin::GLShader *shader = KWin::ShaderManager::instance()->pushShader(KWin::ShaderTrait::MapTexture|KWin::ShaderTrait::UniformColor|KWin::ShaderTrait::Modulate);
        shader->setUniform(KWin::GLShader::ModulationConstant, QVector4D(o, o, o, o));
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        for (int i = 0; i < NTex; ++i)
        {
            QMatrix4x4 modelViewProjection;
            modelViewProjection.ortho(0, s.width(), s.height(), 0, 0, 65535);
            modelViewProjection.translate(rrect[i].x(), rrect[i].y());
            shader->setUniform("modelViewProjectionMatrix", modelViewProjection);
            m_rect[i]->bind();
            m_rect[i]->render(region, rrect[i]);
            m_rect[i]->unbind();
        }
//        KWin::ShaderManager::instance()->popShader();
//        shader = KWin::ShaderManager::instance()->pushShader(KWin::ShaderTrait::UniformColor);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        QRegion reg = QRegion(geo.adjusted(-1, -1, 1, 1)) - geo;
        for (int i = 0; i < 4; ++i)
            reg -= rrect[i];
        fillRegion(reg, QColor(0, 0, 0, m_alpha*data.opacity()));
        fillRegion(QRegion(geo.x()+m_size, geo.y(), geo.width()-m_size*2, 1), QColor(255, 255, 255, m_alpha*data.opacity()));
        KWin::ShaderManager::instance()->popShader();
    }
#endif
    glDisable(GL_BLEND);
}

void
ShapeCornersEffect::fillRegion(const QRegion &reg, const QColor &c)
{
    KWin::GLVertexBuffer *vbo = KWin::GLVertexBuffer::streamingBuffer();
    vbo->reset();
    vbo->setUseColor(true);
    vbo->setColor(c);
    QVector<float> verts;
    foreach (const QRect & r, reg.rects())
    {
        verts << r.x() + r.width() << r.y();
        verts << r.x() << r.y();
        verts << r.x() << r.y() + r.height();
        verts << r.x() << r.y() + r.height();
        verts << r.x() + r.width() << r.y() + r.height();
        verts << r.x() + r.width() << r.y();
    }
    vbo->setData(verts.count() / 2, 2, verts.data(), NULL);
    vbo->render(GL_TRIANGLES);
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

#include "shapecorners.moc"
