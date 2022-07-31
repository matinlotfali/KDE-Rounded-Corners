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
//        qDebug() << "shader valid: " << m_shader->isValid();
        if (m_shader->isValid())
        {
            m_shader_windowActive = m_shader->uniformLocation("windowActive");
            m_shader_windowHasDecoration = m_shader->uniformLocation("windowHasDecoration");
            m_shader_windowSize = m_shader->uniformLocation("windowSize");
            m_shader_shadowColor = m_shader->uniformLocation("shadowColor");
            m_shader_radius = m_shader->uniformLocation("radius");
            m_shader_outlineColor = m_shader->uniformLocation("outlineColor");
            m_shader_outlineThickness = m_shader->uniformLocation("outlineThickness");
            m_shader_sampler = m_shader->uniformLocation("sampler");
            m_shader_back = m_shader->uniformLocation("back");
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

bool isWindowActive(KWin::EffectWindow *w) {
    return KWin::effects->activeWindow() == w;
}

const std::unique_ptr<KWin::GLShader>&
ShaderManager::Bind(KWin::EffectWindow *w, const ConfigModel& config) const {
    m_manager->pushShader(m_shader.get());
    m_shader->setUniform(m_shader_windowActive, isWindowActive(w));
    m_shader->setUniform(m_shader_windowSize, QVector2D(w->frameGeometry().width(), w->frameGeometry().height()));
    m_shader->setUniform(m_shader_windowHasDecoration, w->hasDecoration());
    m_shader->setUniform(m_shader_shadowColor, KWin::effects->hasDecorationShadows()? config.m_shadowColor: QColor(Qt::transparent));
    m_shader->setUniform(m_shader_radius, config.m_size);
    m_shader->setUniform(m_shader_outlineColor, isWindowActive(w) ? config.m_outlineColor : config.m_inactiveOutlineColor);
    m_shader->setUniform(m_shader_outlineThickness, config.m_outlineThickness);
    m_shader->setUniform(m_shader_sampler, 0);
    m_shader->setUniform(m_shader_back, 1);
    return m_shader;
}

const std::unique_ptr<KWin::GLShader>&
ShaderManager::Bind(QMatrix4x4 mvp, KWin::EffectWindow *w, const ConfigModel& config) const {
    Bind(w, config);
    mvp.translate(w->frameGeometry().x(), w->frameGeometry().y());
    m_shader->setUniform(KWin::GLShader::ModelViewProjectionMatrix, mvp);
    return m_shader;
}

void ShaderManager::Unbind() const {
    m_manager->popShader();
}

bool ShaderManager::IsLegacy() {
#ifdef KWIN_HAVE_OPENGLES
    const qint64 coreVersionNumber = kVersionNumber(3, 0);
#else
    const qint64 version = KWin::kVersionNumber(1, 40);
#endif
    return (KWin::GLPlatform::instance()->glslVersion() < version);
}
