//
// Created by matin on 20/07/22.
//

#include <kwinglplatform.h>
#include <QFile>
#include <QStandardPaths>
#include <kwineffects.h>
#include "shapecorners_shader.h"

ShapeCornersShader::ShapeCornersShader():
        m_manager(KWin::ShaderManager::instance())
{
    const QString shadersDir = IsLegacy()? "kwin/shaders/1.10/": "kwin/shaders/1.40/";
    const QString fragmentshader = QStandardPaths::locate(QStandardPaths::GenericDataLocation, shadersDir + QStringLiteral("shapecorners.frag"));
//    m_shader = KWin::ShaderManager::instance()->loadFragmentShader(KWin::ShaderManager::GenericShader, fragmentshader);
    QFile file(fragmentshader);
    if (file.open(QFile::ReadOnly))
    {
        QByteArray frag = file.readAll();
        auto shader = m_manager->generateCustomShader(KWin::ShaderTrait::MapTexture, QByteArray(), frag);
#if KWIN_EFFECT_API_VERSION >= 235
        m_shader = std::move(shader);
#else
        m_shader.reset(shader);
#endif
        file.close();
//        qDebug() << frag;        
        if (m_shader->isValid())
        {
            m_shader_windowHasDecoration = m_shader->uniformLocation("windowHasDecoration");
            m_shader_windowSize = m_shader->uniformLocation("windowSize");
            m_shader_windowExpandedSize = m_shader->uniformLocation("windowExpandedSize");
            m_shader_windowTopLeft = m_shader->uniformLocation("windowTopLeft");
            m_shader_shadowColor = m_shader->uniformLocation("shadowColor");
            m_shader_shadowSize = m_shader->uniformLocation("shadowSize");
            m_shader_radius = m_shader->uniformLocation("radius");
            m_shader_outlineColor = m_shader->uniformLocation("outlineColor");
            m_shader_outlineThickness = m_shader->uniformLocation("outlineThickness");
            m_shader_front = m_shader->uniformLocation("front");
            qInfo() << "ShapeCorners: shaders loaded.";
        }
        else
            qCritical() << "ShapeCorners: no valid shaders found! ShapeCorners will not work.";
    }
    else
    {
        qCritical() << "ShapeCorners: no shaders found! Exiting...";
    }
}

bool ShapeCornersShader::IsValid() const {
    return m_shader && m_shader->isValid();
}

bool isWindowActive(KWin::EffectWindow *w) {
    return KWin::effects->activeWindow() == w;
}

const std::unique_ptr<KWin::GLShader>&
ShapeCornersShader::Bind(KWin::EffectWindow *w, const ConfigModel& config) const {
    auto xy = QVector2D((w->frameGeometry().left() - w->expandedGeometry().left()),
                        (w->frameGeometry().top() - w->expandedGeometry().top()));
    m_manager->pushShader(m_shader.get());
    m_shader->setUniform(m_shader_windowSize, QVector2D(w->frameGeometry().width(), w->frameGeometry().height()));
    m_shader->setUniform(m_shader_windowExpandedSize, QVector2D(w->expandedGeometry().width(), w->expandedGeometry().height()));
    m_shader->setUniform(m_shader_windowTopLeft, xy);
    m_shader->setUniform(m_shader_windowHasDecoration, w->hasDecoration());
    m_shader->setUniform(m_shader_shadowColor, isWindowActive(w) ? config.m_shadowColor: config.m_inactiveShadowColor);
    m_shader->setUniform(m_shader_shadowSize, isWindowActive(w) ? config.m_shadowSize: config.m_inactiveShadowSize);
    m_shader->setUniform(m_shader_radius, config.m_size);
    m_shader->setUniform(m_shader_outlineColor, isWindowActive(w) ? config.m_outlineColor : config.m_inactiveOutlineColor);
    m_shader->setUniform(m_shader_outlineThickness, config.m_outlineThickness);
    m_shader->setUniform(m_shader_front, 0);
    return m_shader;
}

const std::unique_ptr<KWin::GLShader>&
ShapeCornersShader::Bind(QMatrix4x4 mvp, KWin::EffectWindow *w, const ConfigModel& config) const {
    Bind(w, config);
    m_shader->setUniform(KWin::GLShader::ModelViewProjectionMatrix, mvp);
    return m_shader;
}

void ShapeCornersShader::Unbind() const {
    m_manager->popShader();
}

bool ShapeCornersShader::IsLegacy() {
#ifdef KWIN_HAVE_OPENGLES
    const qint64 coreVersionNumber = kVersionNumber(3, 0);
#else
    const qint64 version = KWin::kVersionNumber(1, 40);
#endif
    return (KWin::GLPlatform::instance()->glslVersion() < version);
}
