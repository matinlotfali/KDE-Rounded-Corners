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

#include "Effect.h"

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDebug>

#include "Animation.h"
#include "Config.h"
#include "Utils.h"
#include "Window.h"
#include "WindowManager.h"
#if QT_VERSION_MAJOR >= 6
#include <core/renderviewport.h>
#include <effect/effecthandler.h>
#include <effect/effectwindow.h>
#include <opengl/glutils.h>
#else
#include <kwineffects.h>
#include <kwinglutils.h>
#endif

void ShapeCorners::Effect::WriteBreezeConfig(bool set_disabled)
{
    // Ignore if the last change was less than 10 seconds ago.
    // This is a workaround to prevent infinite loops in X11
    const auto now  = std::chrono::system_clock::now();
    const auto diff = std::chrono::duration_cast<std::chrono::seconds>(now - lastConfigReloadTime).count();
    if (std::abs(diff) < 10) {
        qWarning() << "ShapeCorners: Skipped writing Breeze config"
                   << "because the last change was" << diff << "seconds ago.";
        return;
    }

    const auto cfg      = KSharedConfig::openConfig(QStringLiteral("breezerc"), KConfig::NoGlobals);
    auto       cfgGroup = cfg->group(QStringLiteral("Common"));

    const static auto defaultOutlineIntensity = QStringLiteral("OutlineMedium");
    const static auto defaultRoundedCorners   = QStringLiteral("true");
    const auto        valueOutlineIntensity   = set_disabled ? QStringLiteral("OutlineOff") : defaultOutlineIntensity;
    const auto        valueRoundedCorners     = set_disabled ? QStringLiteral("false") : defaultRoundedCorners;
    const auto entryOutlineIntensity = cfgGroup.readEntry(QStringLiteral("OutlineIntensity"), defaultOutlineIntensity);
    const auto entryRoundedCorners   = cfgGroup.readEntry(QStringLiteral("RoundedCorners"), defaultRoundedCorners);

    if (entryOutlineIntensity == valueOutlineIntensity && entryRoundedCorners == valueRoundedCorners) {
        qWarning() << "ShapeCorners: Skipped writing Breeze config"
                   << "because it is already set.";
        return;
    }

    qInfo() << "ShapeCorners: Writing Breeze config";
    cfgGroup.writeEntry(QStringLiteral("OutlineIntensity"), valueOutlineIntensity);
    cfgGroup.writeEntry(QStringLiteral("RoundedCorners"), valueRoundedCorners);
    cfg->sync();

    QDBusConnection::sessionBus().send(QDBusMessage::createSignal(
            QStringLiteral("/KWin"), QStringLiteral("org.kde.KWin"), QStringLiteral("reloadConfig")));
    lastConfigReloadTime = now;
}

ShapeCorners::Effect::Effect()
{
    // Read configuration and initialize the effect.
    reconfigure(ReconfigureAll);

    // If the shader is valid, create the window manager and connect the windowAdded signal.
    if (m_shaderManager.IsValid()) {
        // Disable Breeze window outline when this effect loads:
        WriteBreezeConfig(true);
        // Create the window manager with the inactive configuration.
        m_windowManager = std::make_unique<WindowManager>();
        // Connect the windowAdded signal to handle new windows.
        connect(KWin::effects, &KWin::EffectsHandler::windowAdded, this, &Effect::windowAdded);
    }
}

ShapeCorners::Effect::~Effect()
{
    // If the shader manager is valid, then the effect has been active, so some cleanup is needed.
    if (m_shaderManager.IsValid()) {
        // Restore Breeze outline setting when effect is unloaded:
        WriteBreezeConfig(false);
    }
};

void ShapeCorners::Effect::reconfigure(const ReconfigureFlags flags)
{
    Q_UNUSED(flags)

    // Reload configuration settings.
    Config::self()->read();

    if (!m_animation) {
        m_animation = std::make_unique<Animation>();
    }
}

void ShapeCorners::Effect::prePaintWindow(KWin::EffectWindow *kwindow, KWin::WindowPrePaintData &data,
                                          std::chrono::milliseconds time)
{
    // Find the managed window structure.
    auto *window = m_windowManager->findWindow(kwindow);

    // If the shader is not valid or the window is not managed or doesn't need the effect, fall back to default.
    if (!m_shaderManager.IsValid() || window == nullptr || !window->hasEffect()) {
        OffscreenEffect::prePaintWindow(kwindow, data, time);
        return;
    }

    // Animate window properties for smooth transitions.
    m_animation->update(*window);
    if (window->isAnimating) {
        kwindow->addRepaintFull();
    }

    // If the window should have rounded corners, adjust the paint and opaque regions.
    if (window->hasRoundCorners()) {
#if QT_VERSION_MAJOR >= 6
        // Calculate geometry and corner size for Qt6.
        const auto geo  = kwindow->frameGeometry() * kwindow->screen()->scale();
        const auto size = window->currentConfig.cornerRadius * kwindow->screen()->scale();
#else
        // Calculate geometry and corner size for Qt5.
        const auto geo  = kwindow->frameGeometry() * KWin::effects->renderTargetScale();
        const auto size = window->currentConfig.cornerRadius * KWin::effects->renderTargetScale();
#endif

        // Create a region for each rounded corner.
        QRegion reg{};
        reg += QRect(geo.x(), geo.y(), size, size);
        reg += QRect(geo.x() + geo.width() - size, geo.y(), size, size);
        reg += QRect(geo.x(), geo.y() + geo.height() - size, size, size);
        reg += QRect(geo.x() + geo.width() - size, geo.y() + geo.height() - size, size, size);

        // Remove the rounded corners from the opaque region and add them to the paint region.
        data.opaque -= reg;
        data.paint += reg;

        // Mark the window as having translucent regions.
        data.setTranslucent();
    }

    // Call the base implementation.
    OffscreenEffect::prePaintWindow(kwindow, data, time);
}

bool ShapeCorners::Effect::supported()
{
    // The effect is supported if OpenGL compositing is enabled.
    return KWin::effects->isOpenGLCompositing();
}

#if QT_VERSION_MAJOR >= 6
void ShapeCorners::Effect::drawWindow(const KWin::RenderTarget &renderTarget, const KWin::RenderViewport &viewport,
                                      KWin::EffectWindow *kwindow, int mask, const QRegion &region,
                                      KWin::WindowPaintData &data)
{
#else
void ShapeCorners::Effect::drawWindow(KWin::EffectWindow *kwindow, int mask, const QRegion &region,
                                      KWin::WindowPaintData &data)
{
#endif
    // Find the managed window structure.
    const auto *window = m_windowManager->findWindow(kwindow);

    // If the shader is not valid or the window is not managed or doesn't need the effect, unredirect and use default
    // drawing.
    if (!m_shaderManager.IsValid() || window == nullptr || !window->hasEffect()) {
        unredirect(kwindow);
#if QT_VERSION_MAJOR >= 6
        OffscreenEffect::drawWindow(renderTarget, viewport, kwindow, mask, region, data);
#else
        OffscreenEffect::drawWindow(kwindow, mask, region, data);
#endif
        return;
    }

#if QT_VERSION_MAJOR >= 6
    // Get the scale factor for Qt6.
    const auto scale = viewport.scale();
#else
    // Get the scale factor for Qt5.
    const auto scale = KWin::effects->renderTargetScale();
#endif

    // Redirect the window for offscreen rendering.
    redirect(kwindow);
    // Set the custom shader for the window.
    setShader(kwindow, m_shaderManager.GetShader().get());
    // Bind the shader with the current window configs and scale.
    m_shaderManager.Bind(*window, scale);
    // Activate the first texture unit which is the window content.
    glActiveTexture(GL_TEXTURE0);

    // Call the base implementation to actually draw the window.
#if QT_VERSION_MAJOR >= 6
    OffscreenEffect::drawWindow(renderTarget, viewport, kwindow, mask, region, data);
#else
    OffscreenEffect::drawWindow(kwindow, mask, region, data);
#endif
    // Unbind the shader after drawing.
    m_shaderManager.Unbind();
}

void ShapeCorners::Effect::windowAdded(KWin::EffectWindow *kwindow)
{
    // Add the new window to the manager.
    if (m_windowManager->addWindow(kwindow)) {
        // Redirect the window and set the shader if it was successfully added.
        redirect(kwindow);
        setShader(kwindow, m_shaderManager.GetShader().get());
    }
}
