
#include "Shader.h"
#include <QFile>
#include <QStandardPaths>
#include <cmath>
#include "Config.h"
#include "Utils.h"
#include "Window.h"
#include "WindowConfig.h"
#if QT_VERSION_MAJOR >= 6
#include <effect/effectwindow.h>
#include <opengl/glutils.h>
#else
#include <kwineffects.h>
#include <kwinglutils.h>
#endif

namespace
{
    /**
     * @brief Converts a QSizeF to a QVector2D.
     * @param size The QSizeF to convert.
     * @return The corresponding QVector2D.
     */
    constexpr QVector2D toVector2D(const QSizeF &size)
    {
        return {static_cast<float>(size.width()), static_cast<float>(size.height())};
    }

    float calculateMagicRatio(const float squircle_power, const float squircle_blend)
    {
        const float circle_dist   = std::sqrt(2.0F);
        const float squircle_dist = std::pow(2.0F, 1.0F / squircle_power);
        const float blended_dist  = circle_dist * (1.0F - squircle_blend) + squircle_dist * squircle_blend;
        const float numerator     = 1.0F - (1.0F / circle_dist);
        const float denominator   = 1.0F - (1.0F / blended_dist);

        return numerator / denominator;
    }

    float cachedMagicRatio(const float squircle_blend)
    {
        static float cached_blend = -1.0F;
        static float cached_ratio = 1.0F;

        if (cached_blend != squircle_blend) {
            cached_blend = squircle_blend;
            cached_ratio = calculateMagicRatio(4.0F, squircle_blend);
        }

        return cached_ratio;
    }
} // namespace

ShapeCorners::Shader::Shader()
{

    qInfo() << "ShapeCorners: loading shaders...";

#ifdef KWIN_X11
    const QString shaderFileRelativePath = QStringLiteral("kwin-x11/shaders/shapecorners.frag");
#else
    const QString shaderFileRelativePath = QStringLiteral("kwin/shaders/shapecorners.frag");
#endif

    // Locate the shader file in the standard data locations
    const QString shaderFilePath = QStandardPaths::locate(QStandardPaths::GenericDataLocation, shaderFileRelativePath);
    // Generate the shader from file using the ShaderManager
    m_shader = KWin::ShaderManager::instance()->generateShaderFromFile(KWin::ShaderTrait::MapTexture, QString(),
                                                                       shaderFilePath);
    if (!IsValid()) {
        qCritical() << "ShapeCorners: no valid shaders found! effect will not work.";
        return;
    }

    // Assign uniform variable locations
    m_shader_windowSize             = m_shader->uniformLocation("windowSize");
    m_shader_windowExpandedSize     = m_shader->uniformLocation("windowExpandedSize");
    m_shader_windowTopLeft          = m_shader->uniformLocation("windowTopLeft");
    m_shader_usesNativeShadows      = m_shader->uniformLocation("usesNativeShadows");
    m_shader_shadowColor            = m_shader->uniformLocation("shadowColor");
    m_shader_shadowSize             = m_shader->uniformLocation("shadowSize");
    m_shader_radius                 = m_shader->uniformLocation("radius");
    m_shader_useSquircleShape       = m_shader->uniformLocation("useSquircleShape");
    m_shader_squircleBlend          = m_shader->uniformLocation("squircleBlend");
    m_shader_squircleMagicRatio     = m_shader->uniformLocation("squircleMagicRatio");
    m_shader_outlineColor           = m_shader->uniformLocation("outlineColor");
    m_shader_outlineThickness       = m_shader->uniformLocation("outlineThickness");
    m_shader_secondOutlineColor     = m_shader->uniformLocation("secondOutlineColor");
    m_shader_secondOutlineThickness = m_shader->uniformLocation("secondOutlineThickness");
    m_shader_outerOutlineColor      = m_shader->uniformLocation("outerOutlineColor");
    m_shader_outerOutlineThickness  = m_shader->uniformLocation("outerOutlineThickness");
    m_shader_front                  = m_shader->uniformLocation("front");
    qInfo() << "ShapeCorners: shaders loaded.";
}

bool ShapeCorners::Shader::IsValid() const
{
#if KWIN_PLUGIN_VERSION_NUM >= QT_VERSION_CHECK(6, 6, 80)
    return m_shader != nullptr;
#else
    return m_shader != nullptr && m_shader->isValid();
#endif
}

void ShapeCorners::Shader::Bind(const Window &window, const double scale) const
{
    if (!IsValid()) {
        return;
    }

    // Calculate scaled geometries
    const auto frameGeometry    = window.w->frameGeometry() * scale;
    const auto expandedGeometry = window.w->expandedGeometry() * scale;
    // Calculate the offset between expanded and frame geometry
    const auto frameOffset = QVector2D(frameGeometry.topLeft() - expandedGeometry.topLeft());
    // Clamp the shadow size to the maximum allowed
    const qreal max_shadow_size = frameOffset.length();
    const auto  shadowSize      = std::min(window.currentConfig.shadowSize * scale, max_shadow_size);
    // Push the shader to the rendering stack
    KWin::ShaderManager::instance()->pushShader(m_shader.get());
    // Set all required uniforms
    m_shader->setUniform(m_shader_windowSize, toVector2D(frameGeometry.size()));
    m_shader->setUniform(m_shader_windowExpandedSize, toVector2D(expandedGeometry.size()));
    m_shader->setUniform(m_shader_windowTopLeft, frameOffset);
    m_shader->setUniform(m_shader_usesNativeShadows, static_cast<int>(Config::useNativeDecorationShadows()));
    m_shader->setUniform(m_shader_useSquircleShape, static_cast<int>(Config::useSquircleShape()));
    const auto squircleBlend = static_cast<float>(Config::squircleness());
    m_shader->setUniform(m_shader_squircleBlend, squircleBlend);
    m_shader->setUniform(m_shader_squircleMagicRatio, cachedMagicRatio(squircleBlend));
    m_shader->setUniform(m_shader_front, 0);
    m_shader->setUniform(m_shader_outlineThickness, static_cast<float>(window.currentConfig.outlineSize * scale));
    m_shader->setUniform(m_shader_secondOutlineThickness,
                         static_cast<float>(window.currentConfig.secondOutlineSize * scale));
    m_shader->setUniform(m_shader_outerOutlineThickness,
                         static_cast<float>(window.currentConfig.outerOutlineSize * scale));
    m_shader->setUniform(m_shader_shadowSize, static_cast<float>(shadowSize));
    m_shader->setUniform(m_shader_shadowColor, window.currentConfig.shadowColor.toQColor());

    // Set shader uniforms based on disabled states
    if (window.hasRoundCorners()) {
        m_shader->setUniform(m_shader_radius, static_cast<float>(window.currentConfig.cornerRadius * scale));
    } else {
        m_shader->setUniform(m_shader_radius, 0.F);
    }
    if (window.hasOutline()) {
        m_shader->setUniform(m_shader_outlineColor, window.currentConfig.outlineColor.toQColor());
        m_shader->setUniform(m_shader_secondOutlineColor, window.currentConfig.secondOutlineColor.toQColor());
        m_shader->setUniform(m_shader_outerOutlineColor, window.currentConfig.outerOutlineColor.toQColor());
    } else {
        m_shader->setUniform(m_shader_outlineColor, QColor(0, 0, 0, 0));
        m_shader->setUniform(m_shader_secondOutlineColor, QColor(0, 0, 0, 0));
        m_shader->setUniform(m_shader_outerOutlineColor, QColor(0, 0, 0, 0));
    }
}

void ShapeCorners::Shader::Unbind() const
{
    if (!IsValid()) {
        return;
    }
    KWin::ShaderManager::instance()->popShader();
}
