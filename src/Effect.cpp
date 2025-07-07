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
#include "Config.h"
#include "Utils.h"
#include "WindowManager.h"
#include "Window.h"
#ifdef QT_DEBUG
#include <QLoggingCategory>
#endif
#if QT_VERSION_MAJOR >= 6
#include <opengl/glutils.h>
#include <effect/effectwindow.h>
#include <effect/effecthandler.h>
#include <core/renderviewport.h>
#else
#include <kwineffects.h>
#include <kwinglutils.h>
#endif

ShapeCorners::Effect::Effect() {
    // Read configuration and initialize the effect.
    reconfigure(ReconfigureAll);

    // If the shader is valid, create the window manager and connect the windowAdded signal.
    if(m_shaderManager.IsValid()) {
        m_windowManager = std::make_unique<WindowManager>();
        connect(KWin::effects, &KWin::EffectsHandler::windowAdded, this, &Effect::windowAdded);
    }
}

ShapeCorners::Effect::~Effect() = default;

void ShapeCorners::Effect::reconfigure(const ReconfigureFlags flags)
{
    Q_UNUSED(flags)
    
    // Reload configuration settings.
    Config::self()->read();
}

void ShapeCorners::Effect::prePaintWindow(KWin::EffectWindow *kwindow, KWin::WindowPrePaintData &data, std::chrono::milliseconds time)
{
    // Find the managed window structure.
    auto* window = m_windowManager->findWindow(kwindow);
    
    // If the shader is not valid or the window is not managed or doesn't need the effect, fall back to default.
    if (!m_shaderManager.IsValid() || window == nullptr || !window->hasEffect())
    {
        OffscreenEffect::prePaintWindow(kwindow, data, time);
        return;
    }

    // Animate window properties for smooth transitions.
    window->animateProperties(time);

    // If the window should have rounded corners, adjust the paint and opaque regions.
    if(window->hasRoundCorners()) {
#if QT_VERSION_MAJOR >= 6
        // Calculate geometry and corner size for Qt6.
        const auto geo = kwindow->frameGeometry() * kwindow->screen()->scale();
        const auto size = window->currentConfig.cornerRadius * kwindow->screen()->scale();
#else
        // Calculate geometry and corner size for Qt5.
        const auto geo = window->frameGeometry() * KWin::effects->renderTargetScale();
        const auto size = window_iterator->second->cornerRadius * KWin::effects->renderTargetScale();
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
                                      KWin::WindowPaintData &data) {
#else
void ShapeCorners::Effect::drawWindow(KWin::EffectWindow *kwindow, int mask, const QRegion &region,
                                      KWin::WindowPaintData &data) {
#endif
    // Find the managed window structure.
    auto* window = m_windowManager->findWindow(kwindow);
    
    // If the shader is not valid or the window is not managed or doesn't need the effect, unredirect and use default drawing.
    if (!m_shaderManager.IsValid() || window == nullptr || !window->hasEffect()) {
        unredirect(kwindow);
#if QT_VERSION_MAJOR >= 6
        OffscreenEffect::drawWindow(renderTarget, viewport, kwindow, mask, region, data);
#else
        OffscreenEffect::drawWindow(window, mask, region, data);
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

void ShapeCorners::Effect::windowAdded(KWin::EffectWindow *kwindow) {
    // Add the new window to the manager.
    if (m_windowManager->addWindow(kwindow)) {
        // Redirect the window and set the shader if it was successfully added.
        redirect(kwindow);
        setShader(kwindow, m_shaderManager.GetShader().get());
    }
}
