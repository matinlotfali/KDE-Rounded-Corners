//
// Created by matin on 20/07/22.
//

#include "Shader.h"
#include "Window.h"
#include "Config.h"
#include <QFile>
#include <QStandardPaths>

#if QT_VERSION_MAJOR >= 6
    #include <effect/effectwindow.h>
    #include <opengl/glutils.h>
#else
    #include <kwineffects.h>
    #include <kwinglutils.h>
#endif

ShapeCorners::Shader::Shader():
        m_manager(KWin::ShaderManager::instance()) {

    qInfo() << "ShapeCorners: loading shaders...";
    const QString shaderFilePath = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                          QStringLiteral("kwin/shaders/shapecorners.frag"));
    m_shader = m_manager->generateShaderFromFile(KWin::ShaderTrait::MapTexture, QString(), shaderFilePath);
    if (!m_shader->isValid()) {
        qCritical() << "ShapeCorners: no valid shaders found! ShapeCorners will not work.";
        return;
    }

    m_shader_windowSize = m_shader->uniformLocation("windowSize");
    m_shader_windowExpandedSize = m_shader->uniformLocation("windowExpandedSize");
    m_shader_windowTopLeft = m_shader->uniformLocation("windowTopLeft");
    m_shader_usesNativeShadows = m_shader->uniformLocation("usesNativeShadows");
    m_shader_shadowColor = m_shader->uniformLocation("shadowColor");
    m_shader_shadowSize = m_shader->uniformLocation("shadowSize");
    m_shader_radius = m_shader->uniformLocation("radius");
    m_shader_outlineColor = m_shader->uniformLocation("outlineColor");
    m_shader_outlineThickness = m_shader->uniformLocation("outlineThickness");
    m_shader_secondOutlineColor = m_shader->uniformLocation("secondOutlineColor");
    m_shader_secondOutlineThickness = m_shader->uniformLocation("secondOutlineThickness");
    m_shader_front = m_shader->uniformLocation("front");
    qInfo() << "ShapeCorners: shaders loaded.";
}

bool ShapeCorners::Shader::IsValid() const {
    return m_shader && m_shader->isValid();
}

void ShapeCorners::Shader::Bind(const Window &window, const qreal scale) const {
    const auto frameGeometry = window.w.frameGeometry() * scale;
    const auto expandedGeometry = window.w.expandedGeometry() * scale;
    const auto xy = QVector2D(frameGeometry.topLeft() - expandedGeometry.topLeft());
    const qreal max_shadow_size = xy.length();
    const auto shadowSize = std::min(window.shadowSize * scale, max_shadow_size);

    m_manager->pushShader(m_shader.get());
    m_shader->setUniform(m_shader_windowSize, toVector2D(frameGeometry.size()));
    m_shader->setUniform(m_shader_windowExpandedSize, toVector2D(expandedGeometry.size()));
    m_shader->setUniform(m_shader_windowTopLeft, xy);
    m_shader->setUniform(m_shader_usesNativeShadows, Config::useNativeDecorationShadows());
    m_shader->setUniform(m_shader_front, 0);
    m_shader->setUniform(m_shader_radius, window.hasRoundCorners()? static_cast<float>(window.cornerRadius * scale): 0);
    m_shader->setUniform(m_shader_outlineThickness, static_cast<float>(window.outlineSize * scale));
    m_shader->setUniform(m_shader_secondOutlineThickness, static_cast<float>(window.secondOutlineSize * scale));
    m_shader->setUniform(m_shader_shadowSize, static_cast<float>(shadowSize));
    m_shader->setUniform(m_shader_outlineColor, window.outlineColor.toQColor());
    m_shader->setUniform(m_shader_secondOutlineColor, window.secondOutlineColor.toQColor());
    m_shader->setUniform(m_shader_shadowColor, window.shadowColor.toQColor());
}

void ShapeCorners::Shader::Unbind() const {
    m_manager->popShader();
}
