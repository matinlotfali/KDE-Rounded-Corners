//
// Created by matin on 20/07/22.
//

#include "ShapeCornersShader.h"
#include "ShapeCornersEffect.h"
#include "ShapeCornersConfig.h"
#include <QWindow>
#include <QFile>
#include <QWidget>

#if QT_VERSION_MAJOR >= 6
    #include <opengl/glutils.h>
#else
    #include <kwinglutils.h>
#endif

ShapeCornersShader::ShapeCornersShader():
        m_manager(KWin::ShaderManager::instance()),
        m_widget(new QWidget)
{
    const QString fragmentshader = QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("kwin/shaders/shapecorners.frag"));
//    m_shader = KWin::ShaderManager::instance()->loadFragmentShader(KWin::ShaderManager::GenericShader, fragmentshader);
    QFile file(fragmentshader);
    if (!file.open(QFile::ReadOnly))
        qCritical() << "ShapeCorners: no shaders found! Exiting...";

    const QByteArray frag = file.readAll();
    auto shader = m_manager->generateShaderFromFile(KWin::ShaderTrait::MapTexture, QStringLiteral(""), fragmentshader);
    m_shader = std::move(shader);
    file.close();
    if (!m_shader->isValid())
        qCritical() << "ShapeCorners: no valid shaders found! ShapeCorners will not work.";

    m_shader_disableRoundedTile = m_shader->uniformLocation("disableRoundedTile");
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
ShapeCornersShader::Bind(KWin::EffectWindow *w, qreal scale, bool isTiled) const {
    QColor outlineColor, shadowColor;
    qreal shadowSize;
    auto& m_palette = m_widget->palette();
    auto frameGeometry = w->frameGeometry() * scale;
    auto expandedGeometry = w->expandedGeometry() * scale;
    auto xy = QVector2D(frameGeometry.topLeft() - expandedGeometry.topLeft());
    auto tiled = (isTiled || (w->internalWindow()->windowStates() & (Qt::WindowMaximized | Qt::WindowFullScreen)));
    qreal max_shadow_size = xy.length();
    m_manager->pushShader(m_shader.get());
    m_shader->setUniform(m_shader_windowSize, toVector2D(frameGeometry.size()));
    m_shader->setUniform(m_shader_windowExpandedSize, toVector2D(expandedGeometry.size()));
    m_shader->setUniform(m_shader_windowTopLeft, xy);
    m_shader->setUniform(m_shader_disableRoundedTile, tiled && ShapeCornersConfig::disableRoundTile());
    m_shader->setUniform(m_shader_front, 0);
    if (ShapeCornersEffect::isWindowActive(w)) {
        shadowSize = std::min(ShapeCornersConfig::shadowSize() * scale, max_shadow_size);
        m_shader->setUniform(m_shader_radius, static_cast<float>(ShapeCornersConfig::size() * scale));
        m_shader->setUniform(m_shader_outlineThickness, static_cast<float>(ShapeCornersConfig::outlineThickness() * scale));

        outlineColor = ShapeCornersConfig::activeOutlineUsePalette() ?
            m_palette.color(QPalette::Active, static_cast<QPalette::ColorRole>(ShapeCornersConfig::activeOutlinePalette())):
            ShapeCornersConfig::outlineColor();
        shadowColor = ShapeCornersConfig::activeShadowUsePalette() ?
            m_palette.color(QPalette::Active, static_cast<QPalette::ColorRole>(ShapeCornersConfig::activeShadowPalette())):
            ShapeCornersConfig::shadowColor();
        outlineColor.setAlpha(tiled && ShapeCornersConfig::disableOutlineTile() ? 0: ShapeCornersConfig::activeOutlineAlpha());
        shadowColor.setAlpha(ShapeCornersConfig::activeShadowAlpha());
    } else {
        shadowSize = std::min(ShapeCornersConfig::inactiveShadowSize() * scale, max_shadow_size);
        m_shader->setUniform(m_shader_radius, static_cast<float>(ShapeCornersConfig::inactiveCornerRadius() * scale));
        m_shader->setUniform(m_shader_outlineThickness, static_cast<float>(ShapeCornersConfig::inactiveOutlineThickness() * scale));

        outlineColor = ShapeCornersConfig::inactiveOutlineUsePalette() ?
                       m_palette.color(QPalette::Inactive, static_cast<QPalette::ColorRole>(ShapeCornersConfig::inactiveOutlinePalette())):
                       ShapeCornersConfig::inactiveOutlineColor();
        shadowColor = ShapeCornersConfig::inactiveShadowUsePalette() ?
                      m_palette.color(QPalette::Inactive, static_cast<QPalette::ColorRole>(ShapeCornersConfig::inactiveShadowPalette())):
                      ShapeCornersConfig::inactiveShadowColor();
        outlineColor.setAlpha(tiled && ShapeCornersConfig::disableOutlineTile() ? 0: ShapeCornersConfig::inactiveOutlineAlpha());
        shadowColor.setAlpha(ShapeCornersConfig::inactiveShadowAlpha());
    }
    m_shader->setUniform(m_shader_shadowSize, static_cast<float>(shadowSize));
    m_shader->setUniform(m_shader_outlineColor, outlineColor);
    m_shader->setUniform(m_shader_shadowColor, shadowColor);
    return m_shader;
}

void ShapeCornersShader::Unbind() const {
    m_manager->popShader();
}
