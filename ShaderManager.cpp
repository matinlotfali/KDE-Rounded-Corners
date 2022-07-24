//
// Created by matin on 20/07/22.
//

#include <kwinglplatform.h>
#include <QFile>
#include <QStandardPaths>
#include <kwineffects.h>
#include "ShaderManager.h"

ShaderManager::ShaderManager():
        m_manager(KWin::ShaderManager::instance())
{
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
        auto shader = m_manager->generateCustomShader(KWin::ShaderTrait::MapTexture, QByteArray(), frag);
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
            m_shader_windowSize = m_shader->uniformLocation("windowSize");
            m_shader_windowActive = m_shader->uniformLocation("windowActive");
            m_shader_shadowColor = m_shader->uniformLocation("shadowColor");
            m_shader_radius = m_shader->uniformLocation("radius");
            m_shader_outlineColor = m_shader->uniformLocation("outlineColor");
            m_shader_outlineThickness = m_shader->uniformLocation("outlineThickness");
        }
        else
            qDebug() << "ShapeCorners: no valid shaders found! ShapeCorners will not work.";
    }
    else
    {
        qDebug() << "ShapeCorners: no shaders found! Exiting...";
    }
}

bool ShaderManager::IsValid() const {
    return m_shader && m_shader->isValid();
}

void ShaderManager::Bind(QMatrix4x4 mvp, const QRect& geo, bool windowActive, const ConfigModel& config) const {
    m_manager->pushShader(m_shader.get());
    mvp.translate((float)geo.x(), (float)geo.y());
    m_shader->setUniform(KWin::GLShader::ModelViewProjectionMatrix, mvp);
    m_shader->setUniform(m_shader_windowSize, QVector2D{(float)geo.width(), (float)geo.height()});
    m_shader->setUniform(m_shader_windowActive, windowActive);
    m_shader->setUniform(m_shader_shadowColor, config.m_shadowColor);
    m_shader->setUniform(m_shader_radius, config.m_size);
    m_shader->setUniform(m_shader_outlineColor, config.m_outlineColor);
    m_shader->setUniform(m_shader_outlineThickness, config.m_outlineThickness);
}

void ShaderManager::Unbind() const {
    m_manager->popShader();
}
