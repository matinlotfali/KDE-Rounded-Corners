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
#include <core/output.h>
#include <core/pixelgrid.h>
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

    const static auto keyOutlineIntensity     = QStringLiteral("OutlineIntensity");
    const static auto keyRoundedCorners       = QStringLiteral("RoundedCorners");
    const static auto keyOutlineEnabled       = QStringLiteral("OutlineEnabled");
    const static auto defaultOutlineIntensity = QStringLiteral("OutlineMedium");
    const static auto defaultRoundedCorners   = QStringLiteral("true");
    const static auto defaultOutlineEnabled   = QStringLiteral("true");
    const auto        valueOutlineIntensity   = set_disabled ? QStringLiteral("OutlineOff") : defaultOutlineIntensity;
    const auto        valueRoundedCorners     = set_disabled ? QStringLiteral("false") : defaultRoundedCorners;
    const auto        valueOutlineEnabled     = set_disabled ? QStringLiteral("false") : defaultOutlineEnabled;
    const auto        entryOutlineIntensity   = cfgGroup.readEntry(keyOutlineIntensity, defaultOutlineIntensity);
    const auto        entryRoundedCorners     = cfgGroup.readEntry(keyRoundedCorners, defaultRoundedCorners);
    const auto        entryOutlineEnabled     = cfgGroup.readEntry(keyOutlineEnabled, defaultOutlineEnabled);

    if (entryOutlineIntensity == valueOutlineIntensity && entryRoundedCorners == valueRoundedCorners &&
        entryOutlineEnabled == valueOutlineEnabled) {
        qWarning() << "ShapeCorners: Skipped writing Breeze config"
                   << "because it is already set.";
        return;
    }

    qInfo() << "ShapeCorners: Writing Breeze config";
    cfgGroup.writeEntry(keyOutlineIntensity, valueOutlineIntensity);
    cfgGroup.writeEntry(keyRoundedCorners, valueRoundedCorners);
    cfgGroup.writeEntry(keyOutlineEnabled, valueOutlineEnabled);
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

#if KWIN_EFFECT_API_VERSION >= 237
#if KWIN_PLUGIN_VERSION_NUM >= QT_VERSION_CHECK(6, 6, 80)
void ShapeCorners::Effect::prePaintWindow(KWin::RenderView *view, KWin::EffectWindow *kwindow,
                                          KWin::WindowPrePaintData &data)
#else
void ShapeCorners::Effect::prePaintWindow(KWin::RenderView *view, KWin::EffectWindow *kwindow,
                                          KWin::WindowPrePaintData &data, std::chrono::milliseconds time)
#endif // KWIN_PLUGIN_VERSION_NUM >= QT_VERSION_CHECK(6, 6, 80)
#else
void ShapeCorners::Effect::prePaintWindow(KWin::EffectWindow *kwindow, KWin::WindowPrePaintData &data,
                                          std::chrono::milliseconds time)
#endif // KWIN_EFFECT_API_VERSION >= 237
{
    // Find the managed window structure.
    auto *window = m_windowManager->findWindow(kwindow);

    // If the shader is not valid or the window is not managed or doesn't need the effect, fall back to default.
    if (!m_shaderManager.IsValid() || window == nullptr || !window->hasEffect()) {
#if KWIN_EFFECT_API_VERSION >= 237
#if KWIN_PLUGIN_VERSION_NUM >= QT_VERSION_CHECK(6, 6, 80)
        OffscreenEffect::prePaintWindow(view, kwindow, data);
#else
        OffscreenEffect::prePaintWindow(view, kwindow, data, time);
#endif // KWIN_PLUGIN_VERSION_NUM >= QT_VERSION_CHECK(6, 6, 80)
#else
        OffscreenEffect::prePaintWindow(kwindow, data, time);
#endif // KWIN_EFFECT_API_VERSION >= 237
        return;
    }

    // Animate window properties for smooth transitions.
    m_animation->update(*window);
    if (window->isAnimating) {
        kwindow->addRepaintFull();
    }

    // If the window should have rounded corners, adjust the paint and opaque regions.
    if (window->hasRoundCorners()) {

#if KWIN_PLUGIN_VERSION_NUM < QT_VERSION_CHECK(6, 6, 80)
#if QT_VERSION_MAJOR >= 6
        // Calculate geometry and corner size for Qt6.
        const auto geo  = (kwindow->frameGeometry() * kwindow->screen()->scale()).toRect();
        const auto size = (int) (window->currentConfig.cornerRadius * kwindow->screen()->scale());
#else
        // Calculate geometry and corner size for Qt5.
        const auto geo  = (kwindow->frameGeometry() * KWin::effects->renderTargetScale()).toRect();
        const auto size = (int) (window->currentConfig.cornerRadius * KWin::effects->renderTargetScale());
#endif // QT_VERSION_MAJOR >= 6

        // Create a region for each rounded corner.
#if KWIN_EFFECT_API_VERSION >= 237
        KWin::Region reg{};
#else
        QRegion reg{};
#endif // KWIN_EFFECT_API_VERSION >= 237
        reg += QRect(geo.x(), geo.y(), size, size);
        reg += QRect(geo.x() + geo.width() - size, geo.y(), size, size);
        reg += QRect(geo.x(), geo.y() + geo.height() - size, size, size);
        reg += QRect(geo.x() + geo.width() - size, geo.y() + geo.height() - size, size, size);

// Remove the rounded corners from the opaque region and add them to the paint region.
#if KWIN_EFFECT_API_VERSION >= 237
        data.deviceOpaque -= reg;
        data.devicePaint += reg;
#else
        data.opaque -= reg;
        data.paint += reg;
#endif // KWIN_EFFECT_API_VERSION >= 237
#endif // KWIN_PLUGIN_VERSION_NUM < QT_VERSION_CHECK(6, 6, 80)

        // Reporting the window as translucent empties its opaque region, so KWin repaints
        // everything behind it. Optional because that costs GPU time, see issue #207.
        if (Config::markWindowsTranslucent()) {
            data.setTranslucent();
        }
    }

    // Call the base implementation.
#if KWIN_EFFECT_API_VERSION >= 237
#if KWIN_PLUGIN_VERSION_NUM >= QT_VERSION_CHECK(6, 6, 80)
    OffscreenEffect::prePaintWindow(view, kwindow, data);
#else
    OffscreenEffect::prePaintWindow(view, kwindow, data, time);
#endif // KWIN_PLUGIN_VERSION_NUM >= QT_VERSION_CHECK(6, 6, 80)
#else
    OffscreenEffect::prePaintWindow(kwindow, data, time);
#endif // KWIN_EFFECT_API_VERSION >= 237
}

bool ShapeCorners::Effect::supported()
{
    // The effect is supported if OpenGL compositing is enabled.
    return KWin::effects->isOpenGLCompositing();
}

#if QT_VERSION_MAJOR >= 6
#if KWIN_EFFECT_API_VERSION >= 237 && KWIN_PLUGIN_VERSION_NUM >= QT_VERSION_CHECK(6, 7, 80)
bool ShapeCorners::Effect::drawWindow(
#else
void ShapeCorners::Effect::drawWindow(
#endif // KWIN_EFFECT_API_VERSION >= 237 && KWIN_PLUGIN_VERSION_NUM >= QT_VERSION_CHECK(6, 7, 80)
        const KWin::RenderTarget &renderTarget, const KWin::RenderViewport &viewport, KWin::EffectWindow *kwindow,
        int mask,
#if KWIN_EFFECT_API_VERSION >= 237
        const KWin::Region &region,
#else
        const QRegion &region,
#endif
        KWin::WindowPaintData &data)
{
#else
void ShapeCorners::Effect::drawWindow(KWin::EffectWindow *kwindow, int mask, const QRegion &region,
                                      KWin::WindowPaintData &data)
{
#endif
    // Forward re-entrant draws to the rest of the chain, see the note on m_windowsBeingDrawn.
    if (m_windowsBeingDrawn.contains(kwindow)) {
#if KWIN_EFFECT_API_VERSION >= 237 && KWIN_PLUGIN_VERSION_NUM >= QT_VERSION_CHECK(6, 7, 80)
        return KWin::effects->drawWindow(renderTarget, viewport, kwindow, mask, region, data);
#elif QT_VERSION_MAJOR >= 6
        KWin::effects->drawWindow(renderTarget, viewport, kwindow, mask, region, data);
        return;
#else
        KWin::effects->drawWindow(kwindow, mask, region, data);
        return;
#endif
    }

    // On llvmpipe/softpipe (software rasterization) with KWin 6.7.x, offscreen rendering through
    // EglSwapchain crashes kwin_wayland whenever a window is resized: OffscreenData::maybeRender()
    // reallocates the dmabuf-backed offscreen texture, and llvmpipe's async worker threads fault in
    // shade_quads while clearing/drawing into the freshly imported buffer. glFinish() cannot help
    // because the crashing work is the *new* offscreen render, not stale references to the old one.
    // The reliable workaround on the effect side is to not redirect windows at all on a broken
    // software renderer: they then composite normally and the buggy offscreen path is never entered.
    // Rounded corners are simply not applied on llvmpipe; a hardware GL renderer is unaffected.
    if (Q_UNLIKELY(isBrokenSoftwareRenderer())) {
        unredirect(kwindow);
#if KWIN_EFFECT_API_VERSION >= 237 && KWIN_PLUGIN_VERSION_NUM >= QT_VERSION_CHECK(6, 7, 80)
        return OffscreenEffect::drawWindow(renderTarget, viewport, kwindow, mask, region, data);
#elif QT_VERSION_MAJOR >= 6
        OffscreenEffect::drawWindow(renderTarget, viewport, kwindow, mask, region, data);
        return;
#else
    OffscreenEffect::drawWindow(kwindow, mask, region, data);
    return;
#endif
    }

    // Find the managed window structure.
    const auto *window = m_windowManager->findWindow(kwindow);

    // If the shader is not valid or the window is not managed or doesn't need the effect, unredirect and use default
    // drawing.
    if (!m_shaderManager.IsValid() || window == nullptr || !window->hasEffect()) {
        // unredirect() destroys the OffscreenData and its EglSwapchain. If that offscreen texture was
        // still being rasterized by llvmpipe (software rendering), freeing it out from under the
        // rasterizer crashes kwin_wayland in shade_quads. Drain the GL pipeline before tearing it down.
        if (m_lastOffscreenSize.remove(kwindow) > 0) {
            glFinish();
        }
        unredirect(kwindow);
#if KWIN_EFFECT_API_VERSION >= 237 && KWIN_PLUGIN_VERSION_NUM >= QT_VERSION_CHECK(6, 7, 80)
        return OffscreenEffect::drawWindow(renderTarget, viewport, kwindow, mask, region, data);
#elif QT_VERSION_MAJOR >= 6
        OffscreenEffect::drawWindow(renderTarget, viewport, kwindow, mask, region, data);
        return;
#else
    OffscreenEffect::drawWindow(kwindow, mask, region, data);
    return;
#endif
    }

#if QT_VERSION_MAJOR >= 6
    // Get the scale factor for Qt6.
    // KWin renders the offscreen texture at the scale of the screen the window belongs to,
    // which is not the scale of the output being painted.
    const auto scale = kwindow->screen()->scale();
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

    // In KWin 6.7.90, OffscreenData::maybeRender() reallocates the EglSwapchain when the offscreen
    // texture size differs. Reassigning m_swapchain destroys the old swapchain and its slots, which
    // frees the old GL textures and their backing dmabuf (software) memory. On llvmpipe that memory
    // is freed immediately, but the previous frame's paint() drew the offscreen texture onto the
    // screen on the main compositor context. Its EGLNativeFence is created on the swapchain's
    // EGLDisplay, and on Mesa/llvmpipe it does not reliably drain the main context's queued draw
    // commands. Those commands still sample the old offscreen texture, so when the swapchain is
    // recreated the rasterization threads dereference freed memory and crash in shade_quads.
    //
    // We track the offscreen texture size with the exact same formula KWin uses in
    // OffscreenData::maybeRender() (offscreeneffect.cpp):
    //     snapToPixels(window->expandedGeometry(), scale).size() * scale
    // and glFinish() the moment it changes. glFinish() drains the whole llvmpipe pipeline, so by
    // the time maybeRender() destroys the old swapchain its texture is no longer in use. This only
    // triggers when the window size actually changes (e.g. during a resize drag), not on every frame.
#if QT_VERSION_MAJOR >= 6
    const auto snappedSize = KWin::snapToPixels(kwindow->expandedGeometry(), scale).size() * scale;
    const QSizeF offscreenSize(snappedSize.width(), snappedSize.height());
#else
    const QSizeF offscreenSize = kwindow->expandedGeometry().size();
#endif
    if (m_lastOffscreenSize.value(kwindow) != offscreenSize) {
        glFinish();
    }
    m_lastOffscreenSize.insert(kwindow, offscreenSize);

    // Call the base implementation to actually draw the window.
    m_windowsBeingDrawn.insert(kwindow);
#if KWIN_EFFECT_API_VERSION >= 237 && KWIN_PLUGIN_VERSION_NUM >= QT_VERSION_CHECK(6, 7, 80)
    const bool result = OffscreenEffect::drawWindow(renderTarget, viewport, kwindow, mask, region, data);
#elif QT_VERSION_MAJOR >= 6
    OffscreenEffect::drawWindow(renderTarget, viewport, kwindow, mask, region, data);
#else
OffscreenEffect::drawWindow(kwindow, mask, region, data);
#endif
    m_windowsBeingDrawn.erase(kwindow);
    // Unbind the shader after drawing.
    m_shaderManager.Unbind();
#if KWIN_EFFECT_API_VERSION >= 237 && KWIN_PLUGIN_VERSION_NUM >= QT_VERSION_CHECK(6, 7, 80)
    return result;
#endif
}

void ShapeCorners::Effect::windowAdded(KWin::EffectWindow *kwindow)
{
    // On a broken software renderer we never enter the offscreen path, so don't redirect either.
    if (Q_UNLIKELY(isBrokenSoftwareRenderer())) {
        return;
    }
    // Add the new window to the manager.
    if (m_windowManager->addWindow(kwindow)) {
        // Redirect the window and set the shader if it was successfully added.
        redirect(kwindow);
        setShader(kwindow, m_shaderManager.GetShader().get());
    }
}

bool ShapeCorners::Effect::isBrokenSoftwareRenderer() const
{
    // Query once; the renderer string cannot change during a session.
    if (m_softwareRendererChecked) {
        return m_brokenSoftwareRenderer;
    }
    m_softwareRendererChecked = true;

#if QT_VERSION_MAJOR >= 6
    // GL is guaranteed to be current here: drawWindow runs inside the compositing cycle.
    const char *renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
    if (renderer) {
        const QString r = QString::fromUtf8(renderer).toLower();
        // llvmpipe reports "llvmpipe (LLVM ..., N x86_64)"; softpipe is the older reference rasterizer.
        m_brokenSoftwareRenderer = r.contains(QLatin1String("llvmpipe")) || r.contains(QLatin1String("softpipe"));
    }
    if (m_brokenSoftwareRenderer) {
        qWarning() << "ShapeCorners: llvmpipe/software renderer detected; offscreen rounding disabled"
                      "to avoid kwin_wayland crashes on resize (KWin 6.7.x).";
    }
#endif
    return m_brokenSoftwareRenderer;
}
