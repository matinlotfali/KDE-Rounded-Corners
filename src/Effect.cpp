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

#include <QtDBus/QDBusConnection>
#include <QDBusError>
#include <KX11Extras>

#if QT_VERSION_MAJOR >= 6
    #include <opengl/glutils.h>
    #include <effect/effectwindow.h>
    #include <effect/effecthandler.h>
    #include <core/renderviewport.h>
#else
    #include <kwineffects.h>
    #include <kwinglutils.h>
#endif


ShapeCorners::Effect::Effect()
    : KWin::OffscreenEffect()
{
    reconfigure(ReconfigureAll);

    if (auto connection = QDBusConnection::sessionBus();
        !connection.isConnected()) {
        qWarning() << "ShapeCorners: Cannot connect to the D-Bus session bus.";
    }
    else {
        if (!connection.registerService(QStringLiteral("org.kde.ShapeCorners"))) {
            qWarning() << "ShapeCorners:" << connection.lastError().message();
        }
        else {
            if (!connection.registerObject(QStringLiteral("/ShapeCornersEffect"), this, QDBusConnection::ExportAllSlots)) {
                qWarning() << "ShapeCorners:" << connection.lastError().message();
            }
        }
    }

    if(m_shaderManager.IsValid()) {
        for (const auto& win: KWin::effects->stackingOrder())
            windowAdded(win);
        connect(KWin::effects, &KWin::EffectsHandler::windowAdded, this, &Effect::windowAdded);
        connect(KWin::effects, &KWin::EffectsHandler::windowDeleted, this, &Effect::windowRemoved);
    }
}

ShapeCorners::Effect::~Effect() = default;

void
ShapeCorners::Effect::windowAdded(KWin::EffectWindow *w)
{
#ifdef QT_DEBUG
    qInfo() << "ShapeCorners: window added" << w->windowClass() << "type" << w->windowType() << "role" << w->windowRole();
#endif
    
    if (w->windowClass().trimmed().isEmpty()) {
#ifdef QT_DEBUG
        qWarning() << "ShapeCorners: window does not have a valid class name.";
#endif
        return;
    }

    const QSet<QString> hardExceptions {
        "kwin", "kwin_x11", "kwin_wayland", "kscreenlocker_greet", "ksmserver", "krunner"
    };
    const auto name = w->windowClass().split(QChar::Space).first();
    if (hardExceptions.contains(name)) {
#ifdef QT_DEBUG
        qWarning() << "ShapeCorners: ignoring window explicitly.";
#endif
        return;
    }

    if (const auto& [w2, r] = m_managed.insert({w, ShapeCorners::Window(w, name)}); !r) {
#ifdef QT_DEBUG
        qWarning() << "ShapeCorners: ignoring duplicate window.";
#endif
        return;
    }

#ifdef QT_DEBUG
    if (Config::exclusions().contains(name)) {
        qWarning() << "ShapeCorners: window is excluded in configurations.";
    }
#endif

#if QT_VERSION_MAJOR >= 6
    connect(w, &KWin::EffectWindow::windowFrameGeometryChanged, this, &Effect::windowResized);
#else
    connect(KWin::effects, &KWin::EffectsHandler::windowFrameGeometryChanged, this, &Effect::windowResized);
#endif
    redirect(w);
    setShader(w, m_shaderManager.GetShader().get());
    checkTiled();
    checkMaximized(w);
}

void ShapeCorners::Effect::windowRemoved(KWin::EffectWindow *w)
{
    auto window_iterator = m_managed.find(w);
    if (window_iterator != m_managed.end()) {
        qDebug() << "ShapeCorners: window removed" << window_iterator->second.name;
        m_managed.erase(window_iterator);
    } else {
        qDebug() << "ShapeCorners: window removed";
    }
    checkTiled();
}

void
ShapeCorners::Effect::reconfigure(const ReconfigureFlags flags)
{
    Q_UNUSED(flags)
    Config::self()->read();
}

void ShapeCorners::Effect::prePaintWindow(KWin::EffectWindow *w, KWin::WindowPrePaintData &data, std::chrono::milliseconds time)
{
    auto window_iterator = m_managed.find(w);
    if (!m_shaderManager.IsValid()
        || window_iterator == m_managed.end()
        || !window_iterator->second.hasEffect())
    {
        OffscreenEffect::prePaintWindow(w, data, time);
        return;
    }

    window_iterator->second.animateProperties(time);

    if(window_iterator->second.hasRoundCorners()) {
#if QT_VERSION_MAJOR >= 6
        const auto geo = w->frameGeometry() * w->screen()->scale();
        const auto size = window_iterator->second.cornerRadius * w->screen()->scale();
#else
        const auto geo = w->frameGeometry() * KWin::effects->renderTargetScale();
        const auto size = window_iterator->second.cornerRadius * KWin::effects->renderTargetScale();
#endif

        QRegion reg{};
        reg += QRect(geo.x(), geo.y(), size, size);
        reg += QRect(geo.x() + geo.width() - size, geo.y(), size, size);
        reg += QRect(geo.x(), geo.y() + geo.height() - size, size, size);
        reg += QRect(geo.x() + geo.width() - size, geo.y() + geo.height() - size, size, size);
        data.opaque -= reg;
        data.paint += reg;
        data.setTranslucent();
    }

    OffscreenEffect::prePaintWindow(w, data, time);
}

bool ShapeCorners::Effect::supported()
{
    return KWin::effects->isOpenGLCompositing();
}

#if QT_VERSION_MAJOR >= 6
void ShapeCorners::Effect::drawWindow(const KWin::RenderTarget &renderTarget, const KWin::RenderViewport &viewport,
                                    KWin::EffectWindow *w, int mask, const QRegion &region,
                                    KWin::WindowPaintData &data) {
#else
void ShapeCorners::Effect::drawWindow(KWin::EffectWindow *w, int mask, const QRegion &region,
                                    KWin::WindowPaintData &data) {
#endif
    auto window_iterator = m_managed.find(w);
    if (!m_shaderManager.IsValid()
        || window_iterator == m_managed.end()
        || !window_iterator->second.hasEffect())
    {
        unredirect(w);
#if QT_VERSION_MAJOR >= 6
        OffscreenEffect::drawWindow(renderTarget, viewport, w, mask, region, data);
#else
        OffscreenEffect::drawWindow(w, mask, region, data);
#endif
        return;
    }

#if QT_VERSION_MAJOR >= 6
    const auto scale = viewport.scale();
#else
    const auto scale = KWin::effects->renderTargetScale();
#endif

    redirect(w);
    setShader(w, m_shaderManager.GetShader().get());
    m_shaderManager.Bind(window_iterator->second, scale);
    glActiveTexture(GL_TEXTURE0);

#if QT_VERSION_MAJOR >= 6
    OffscreenEffect::drawWindow(renderTarget, viewport, w, mask, region, data);
#else
    OffscreenEffect::drawWindow(w, mask, region, data);
#endif
    m_shaderManager.Unbind();
}

QString ShapeCorners::Effect::get_window_titles() const {
    QStringList response;
    for (const auto& [w, window]: m_managed) {
        if (!response.contains(window.name))
            response.push_back(window.name);
    }
    return response.join("\n");
}

void ShapeCorners::Effect::checkTiled() {
    TileChecker tileChecker (m_managed);
    tileChecker.clearTiles();

    if (!Config::disableRoundTile() && !Config::disableOutlineTile()) {
        return;
    }

    for (const auto& screen: KWin::effects->screens()) {        // Per every screen
        const auto& geometry = screen->geometry();
        tileChecker.checkTiles(geometry);
    }
}

void ShapeCorners::Effect::checkMaximized(KWin::EffectWindow *w) {
    auto window_iterator = m_managed.find(w);
    if (window_iterator == m_managed.end())
        return;

    window_iterator->second.isMaximized = false;

    auto screen_region = QRegion(w->screen()->geometry());
    qDebug() << "ShapeCorners: screen region" << screen_region;

    // subtract all menus
    for (auto& [ptr, window]: m_managed)
        if (ptr->isDock()) {
            qDebug() << "ShapeCorners: menu is" << ptr->frameGeometry();
            screen_region -= ptr->frameGeometry().toRect();
        }
    qDebug() << "ShapeCorners: screen region without menus" << screen_region;

    // check if window and screen match
    auto remaining = screen_region - w->frameGeometry().toRect();
    qDebug() << "ShapeCorners: active window remaining region" << remaining;
    if (remaining.isEmpty()) {
        window_iterator->second.isMaximized = true;
#ifdef QT_DEBUG
        qInfo() << "ShapeCorners: window maximized" << window_iterator->second.name;
#endif
    }
}

void ShapeCorners::Effect::windowResized(KWin::EffectWindow *window, const QRectF &)
{
    checkTiled();
    checkMaximized(window);
}
