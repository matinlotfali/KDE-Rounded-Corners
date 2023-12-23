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

#include <QtDBus/QDBusConnection>
#include <QDBusError>
#include <kwinglutils.h>
#include "ShapeCornersEffect.h"
#include "ShapeCornersConfig.h"

#if KWIN_EFFECT_API_VERSION >= 235
#include <KX11Extras>
#else
#include <kwindowsystem.h>
#endif

ShapeCornersEffect::ShapeCornersEffect()
#if KWIN_EFFECT_API_VERSION >= 236
    : KWin::OffscreenEffect()
#else
    : KWin::DeformEffect()
#endif
{
    reconfigure(ReconfigureAll);

    if (auto connection = QDBusConnection::sessionBus();
        !connection.isConnected()) {
        qWarning("ShapeCorners: Cannot connect to the D-Bus session bus.\n");
    }
    else {
        if (!connection.registerService("org.kde.ShapeCorners")) {
            qWarning("%s\n", qPrintable(connection.lastError().message()));
        }
        else {
            if (!connection.registerObject("/ShapeCornersEffect",  this, QDBusConnection::ExportAllSlots)) {
                qWarning("%s\n", qPrintable(connection.lastError().message()));
            }
        }
    }

    if(m_shaderManager.IsValid()) {
#if KWIN_EFFECT_API_VERSION >= 235
        const auto& windowList = KX11Extras::windows();
#else
        const auto& windowList = KWindowSystem::windows();
#endif
        for (const auto& id: windowList)
            if (const auto win = KWin::effects->findWindow(id))
                windowAdded(win);
        connect(KWin::effects, &KWin::EffectsHandler::windowAdded, this, &ShapeCornersEffect::windowAdded);
        connect(KWin::effects, &KWin::EffectsHandler::windowDeleted, this, &ShapeCornersEffect::windowRemoved);
    }
}

ShapeCornersEffect::~ShapeCornersEffect() = default;

void
ShapeCornersEffect::windowAdded(KWin::EffectWindow *w)
{
    qDebug() << w->windowRole() << w->windowType() << w->windowClass();
    if (const auto& [w2, r] = m_managed.insert(w); r) {
        redirect(w);
        setShader(w, m_shaderManager.GetShader().get());
    }
}

void ShapeCornersEffect::windowRemoved(KWin::EffectWindow *w)
{
    m_managed.erase(w);
    unredirect(w);
}

void
ShapeCornersEffect::reconfigure(const ReconfigureFlags flags)
{
    Q_UNUSED(flags)
    ShapeCornersConfig::self()->read();
}

bool ShapeCornersEffect::isMaximized(const KWin::EffectWindow *w) {
    if (w == nullptr)
        return false;

    const auto& screen = KWin::effects->findScreen(w->screen()->name());
    if (screen == nullptr)
        return false;

    const auto& screenGeometry = screen->geometry();
    return (w->x() == screenGeometry.x() && w->width() == screenGeometry.width()) ||
           (w->y() == screenGeometry.y() && w->height() == screenGeometry.height());
}

inline QRectF operator *(const QRect& r, const qreal scale) { return {r.x() * scale, r.y() * scale, r.width() * scale, r.height() * scale}; }
inline QRectF operator *(const QRectF& r, const qreal scale) { return {r.x() * scale, r.y() * scale, r.width() * scale, r.height() * scale}; }
inline QRect toRect(const QRectF& r) { return {static_cast<int>(r.x()), static_cast<int>(r.y()), static_cast<int>(r.width()), static_cast<int>(r.height())}; }
inline const QRect& toRect(const QRect& r) { return r; }

void ShapeCornersEffect::prePaintWindow(KWin::EffectWindow *w, KWin::WindowPrePaintData &data, std::chrono::milliseconds time)
{
    if (!hasEffect(w))
    {
        Effect::prePaintWindow(w, data, time);
        return;
    }

    const auto size = isWindowActive(w) ? ShapeCornersConfig::size(): ShapeCornersConfig::inactiveCornerRadius();

#if KWIN_EFFECT_API_VERSION >= 234
    const auto geo = w->frameGeometry() * KWin::effects->renderTargetScale();
    data.setTranslucent();
#else
    const auto& geo = w->expandedGeometry();
#endif
    QRegion reg {};
    reg += QRect(geo.x(), geo.y(), size, size);
    reg += QRect(geo.x()+geo.width()-size, geo.y(), size, size);
    reg += QRect(geo.x(), geo.y()+geo.height()-size, size, size);
    reg += QRect(geo.x()+geo.width()-size, geo.y()+geo.height()-size, size, size);
#if KWIN_EFFECT_API_VERSION >= 234
    data.opaque -= reg;
#endif
    data.paint += reg;

#if KWIN_EFFECT_API_VERSION >= 236
    OffscreenEffect::prePaintWindow(w, data, time);
#else
    DeformEffect::prePaintWindow(w, data, time);
#endif
}

bool ShapeCornersEffect::supported()
{
    return KWin::effects->isOpenGLCompositing();
}

void ShapeCornersEffect::drawWindow(KWin::EffectWindow *w, int mask, const QRegion &region,
                                    KWin::WindowPaintData &data) {
    if (!hasEffect(w))
    {
        unredirect(w);
#if KWIN_EFFECT_API_VERSION >= 236
        OffscreenEffect::drawWindow(w, mask, region, data);
#else
        DeformEffect::drawWindow(w, mask, region, data);
#endif
        return;
    }

    redirect(w);
    setShader(w, m_shaderManager.GetShader().get());
    m_shaderManager.Bind(w);
    glActiveTexture(GL_TEXTURE0);

#if KWIN_EFFECT_API_VERSION >= 236
    OffscreenEffect::drawWindow(w, mask, region, data);
#else
    DeformEffect::drawWindow(w, mask, region, data);
#endif
    m_shaderManager.Unbind();
}

bool ShapeCornersEffect::hasEffect(const KWin::EffectWindow *w) const {
    const auto name = w->windowClass().split(' ').first();
    return m_shaderManager.IsValid()
           && enabled()
           && m_managed.contains(w)
           && (w->hasDecoration() || (w->isNormalWindow() && ShapeCornersConfig::inclusions().contains(name)))
           && !ShapeCornersConfig::exclusions().contains(name)
           && !(ShapeCornersConfig::excludeMaximizedWindows() && isMaximized(w));
}

QString ShapeCornersEffect::get_window_titles() const {
    QSet<QString> response;
    for (const auto& win: m_managed) {
        const auto name = win->windowClass().split(' ').first();
        if (name == "plasmashell")
            continue;
        response.insert(name);
    }
    return response.values().join("\n");
}

bool ShapeCornersEffect::enabled() {
    if (ShapeCornersConfig::gameMode() && isMaximized(KWin::effects->activeWindow()))
        return false;
    return true;
}
