
#include "Shader.h"
#include "Window.h"
#include "Config.h"
#include "Utils.h"
#include <QFile>
#include <QStandardPaths>
#if QT_VERSION_MAJOR >= 6
#include <effect/effectwindow.h>
#include <opengl/glutils.h>
#else
#include <kwineffects.h>
#include <kwinglutils.h>
#endif

namespace ShapeCorners {
    /**
     * @brief Converts a QSizeF to a QVector2D.
     * @param size The QSizeF to convert.
     * @return The corresponding QVector2D.
     */
    constexpr QVector2D toVector2D(const QSizeF &size) noexcept {
        return {
            static_cast<float>(size.width()),
            static_cast<float>(size.height())
        };
    }
}

/**
 * \brief An instance of `ShaderManager` used to load shader from file and push/pop the shader for each render.
 */
static KWin::ShaderManager *m_manager = KWin::ShaderManager::instance();

ShapeCorners::Shader::Shader() {

    qInfo() << "ShapeCorners: loading shaders...";

    // Locate the shader file in the standard data locations
    const QString shaderFilePath = QStandardPaths::locate(QStandardPaths::GenericDataLocation,
                                                          QStringLiteral("kwin/shaders/shapecorners.frag"));
    // Generate the shader from file using the ShaderManager
    m_shader = m_manager->generateShaderFromFile(KWin::ShaderTrait::MapTexture, QString(), shaderFilePath);
    if (!m_shader->isValid()) {
        qCritical() << "ShapeCorners: no valid shaders found! effect will not work.";
        return;
    }

    // Assign uniform variable locations
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

bool ShapeCorners::Shader::IsValid() const noexcept {
    return m_shader && m_shader->isValid();
}

void ShapeCorners::Shader::Bind(const Window &window, const double scale) const noexcept {
    if (!IsValid()) {
        return;
    }

    // Calculate scaled geometries
    const auto frameGeometry = window.w->frameGeometry() * scale;
    const auto expandedGeometry = window.w->expandedGeometry() * scale;
    // Calculate the offset between expanded and frame geometry
    const auto frameOffset = QVector2D(frameGeometry.topLeft() - expandedGeometry.topLeft());
    // Clamp the shadow size to the maximum allowed
    const qreal max_shadow_size = frameOffset.length();
    const auto shadowSize = std::min(window.currentConfig.shadowSize * scale, max_shadow_size);

    // Push the shader to the rendering stack
    m_manager->pushShader(m_shader.get());
    // Set all required uniforms
    m_shader->setUniform(m_shader_windowSize, toVector2D(frameGeometry.size()));
    m_shader->setUniform(m_shader_windowExpandedSize, toVector2D(expandedGeometry.size()));
    m_shader->setUniform(m_shader_windowTopLeft, frameOffset);
    m_shader->setUniform(m_shader_usesNativeShadows, static_cast<int>(Config::useNativeDecorationShadows()));
    m_shader->setUniform(m_shader_front, 0);
    m_shader->setUniform(m_shader_radius, static_cast<float>(window.currentConfig.cornerRadius * scale));
    m_shader->setUniform(m_shader_outlineThickness, static_cast<float>(window.currentConfig.outlineSize * scale));
    m_shader->setUniform(m_shader_secondOutlineThickness, static_cast<float>(window.currentConfig.secondOutlineSize * scale));
    m_shader->setUniform(m_shader_shadowSize, static_cast<float>(shadowSize));
    m_shader->setUniform(m_shader_outlineColor, window.currentConfig.outlineColor.toQColor());
    m_shader->setUniform(m_shader_secondOutlineColor, window.currentConfig.secondOutlineColor.toQColor());
    m_shader->setUniform(m_shader_shadowColor, window.currentConfig.shadowColor.toQColor());
}

void ShapeCorners::Shader::Unbind() const noexcept {
    if (!IsValid()) {
        return;
    }
    m_manager->popShader();
}
