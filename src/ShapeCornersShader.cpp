//
// Created by matin on 20/07/22.
//

#include <kwinglplatform.h>
#include <QFile>
#include <QStandardPaths>
#include <kwineffects.h>
#include <QWidget>
#include "ShapeCornersEffect.h"
#include "ShapeCornersShader.h"

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

const std::unique_ptr<KWin::GLShader>&
ShapeCornersShader::Bind(KWin::EffectWindow *w) const {
    QColor outlineColor, shadowColor;
    auto& m_palette = m_widget.palette();
    auto xy = QVector2D((w->frameGeometry().left() - w->expandedGeometry().left()),
                        (w->frameGeometry().top() - w->expandedGeometry().top()));
    m_manager->pushShader(m_shader.get());
    m_shader->setUniform(m_shader_windowSize, QVector2D(w->frameGeometry().width(), w->frameGeometry().height()));
    m_shader->setUniform(m_shader_windowExpandedSize, QVector2D(w->expandedGeometry().width(), w->expandedGeometry().height()));
    m_shader->setUniform(m_shader_windowTopLeft, xy);
    m_shader->setUniform(m_shader_front, 0);
    if (ShapeCornersEffect::isWindowActive(w)) {
        m_shader->setUniform(m_shader_shadowSize, (float)ShapeCornersConfig::shadowSize());
        m_shader->setUniform(m_shader_radius, (float)ShapeCornersConfig::size());
        m_shader->setUniform(m_shader_outlineThickness, (float)ShapeCornersConfig::outlineThickness());

        outlineColor = ShapeCornersConfig::activeOutlineUsePalette() ?
            m_palette.color(QPalette::Active, static_cast<QPalette::ColorRole>(ShapeCornersConfig::activeOutlinePalette())):
            ShapeCornersConfig::outlineColor();
        shadowColor = ShapeCornersConfig::activeShadowUsePalette() ?
            m_palette.color(QPalette::Active, static_cast<QPalette::ColorRole>(ShapeCornersConfig::activeShadowPalette())):
            ShapeCornersConfig::shadowColor();
        outlineColor.setAlpha(ShapeCornersConfig::activeOutlineAlpha());
        shadowColor.setAlpha(ShapeCornersConfig::activeShadowAlpha());
    } else {
        m_shader->setUniform(m_shader_shadowSize, (float)ShapeCornersConfig::inactiveShadowSize());
        m_shader->setUniform(m_shader_radius, (float)ShapeCornersConfig::inactiveCornerRadius());
        m_shader->setUniform(m_shader_outlineThickness, (float)ShapeCornersConfig::inactiveOutlineThickness());

        outlineColor = ShapeCornersConfig::inactiveOutlineUsePalette() ?
                       m_palette.color(QPalette::Active, static_cast<QPalette::ColorRole>(ShapeCornersConfig::inactiveOutlinePalette())):
                       ShapeCornersConfig::inactiveOutlineColor();
        shadowColor = ShapeCornersConfig::inactiveShadowUsePalette() ?
                      m_palette.color(QPalette::Active, static_cast<QPalette::ColorRole>(ShapeCornersConfig::inactiveShadowPalette())):
                      ShapeCornersConfig::inactiveShadowColor();
        outlineColor.setAlpha(ShapeCornersConfig::inactiveOutlineAlpha());
        shadowColor.setAlpha(ShapeCornersConfig::inactiveShadowAlpha());
    }
    m_shader->setUniform(m_shader_outlineColor, outlineColor);
    m_shader->setUniform(m_shader_shadowColor, shadowColor);
    return m_shader;
}

const std::unique_ptr<KWin::GLShader>&
ShapeCornersShader::Bind(QMatrix4x4 mvp, KWin::EffectWindow *w) const {
    Bind(w);
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
