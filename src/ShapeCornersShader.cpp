//
// Created by matin on 20/07/22.
//

#include "ShapeCornersShader.h"
#include <QFile>
#include <QStandardPaths>

#if QT_VERSION_MAJOR >= 6
    #include <effect/effectwindow.h>
    #include <opengl/glutils.h>
#else
    #include <kwineffects.h>
    #include <kwinglutils.h>
#endif

ShapeCornersShader::ShapeCornersShader():
        m_manager(KWin::ShaderManager::instance()) {

    qInfo() << "ShapeCorners: loading shaders...";
    const QString shaderFilePath = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                          QStringLiteral("kwin/shaders/shapecorners.frag"));
    m_shader = m_manager->generateShaderFromFile(KWin::ShaderTrait::MapTexture, QString(), shaderFilePath);
    if (!m_shader->isValid())
        qCritical() << "ShapeCorners: no valid shaders found! ShapeCorners will not work.";

    m_shader_hasRoundCorners = m_shader->uniformLocation("hasRoundCorners");
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

bool ShapeCornersShader::IsValid() const {
    return m_shader && m_shader->isValid();
}

const std::unique_ptr<KWin::GLShader>&
ShapeCornersShader::Bind(const ShapeCornersWindow &window, qreal scale) const {
    auto frameGeometry = window.w->frameGeometry() * scale;
    auto expandedGeometry = window.w->expandedGeometry() * scale;
    auto xy = QVector2D(frameGeometry.topLeft() - expandedGeometry.topLeft());
    qreal max_shadow_size = xy.length();
    m_manager->pushShader(m_shader.get());
    m_shader->setUniform(m_shader_windowSize, toVector2D(frameGeometry.size()));
    m_shader->setUniform(m_shader_windowExpandedSize, toVector2D(expandedGeometry.size()));
    m_shader->setUniform(m_shader_windowTopLeft, xy);
    m_shader->setUniform(m_shader_hasRoundCorners, window.hasRoundCorners());
    m_shader->setUniform(m_shader_front, 0);
    m_shader->setUniform(m_shader_radius, static_cast<float>(window.cornerRadius * scale));
    m_shader->setUniform(m_shader_outlineThickness, static_cast<float>(window.outlineSize * scale));
    auto shadowSize = std::min(window.shadowSize * scale, max_shadow_size);
    m_shader->setUniform(m_shader_shadowSize, static_cast<float>(shadowSize));
    m_shader->setUniform(m_shader_outlineColor, window.outlineColor.toQColor());
    m_shader->setUniform(m_shader_shadowColor, window.shadowColor.toQColor());
    return m_shader;
}

void ShapeCornersShader::Unbind() const {
    m_manager->popShader();
}
