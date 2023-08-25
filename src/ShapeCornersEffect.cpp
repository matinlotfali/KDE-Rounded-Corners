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

#include "ShapeCornersEffect.h"
#include <kwingltexture.h>
#include <QtDBus/QDBusConnection>
#include <QDBusError>

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

    auto connection = QDBusConnection::sessionBus();
    if (!connection.isConnected()) {
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
            if (auto win = KWin::effects->findWindow(id))
                windowAdded(win);
        connect(KWin::effects, &KWin::EffectsHandler::windowAdded, this, &ShapeCornersEffect::windowAdded);
        connect(KWin::effects, &KWin::EffectsHandler::windowDeleted, this, &ShapeCornersEffect::windowRemoved);
        connect(KWin::effects, &KWin::EffectsHandler::windowFinishUserMovedResized, this, &ShapeCornersEffect::windowResized);
        connect(KWin::effects, &KWin::EffectsHandler::windowMaximizedStateChanged, this, &ShapeCornersEffect::windowResized);
    }
}

ShapeCornersEffect::~ShapeCornersEffect() = default;

void
ShapeCornersEffect::windowAdded(KWin::EffectWindow *w)
{
    qDebug() << w->windowRole() << w->windowType() << w->windowClass();
    auto [it, success] = m_managed.insert({w, false});
    if (success) {
        redirect(w);
        setShader(w, m_shaderManager.GetShader().get());
        if (w->width() >= 300 && w->height() >= 300)
            checkTiled();
    }
}

void ShapeCornersEffect::windowRemoved(KWin::EffectWindow *w)
{
    m_managed.erase(w);
    unredirect(w);
    if (w->width() >= 300 && w->height() >= 300)
        checkTiled();
}

void
ShapeCornersEffect::reconfigure(ReconfigureFlags flags)
{
    Q_UNUSED(flags)
    ShapeCornersConfig::self()->read();
}

QRectF operator *(QRect r, qreal scale) { return {r.x() * scale, r.y() * scale, r.width() * scale, r.height() * scale}; }
QRectF operator *(QRectF r, qreal scale) { return {r.x() * scale, r.y() * scale, r.width() * scale, r.height() * scale}; }
QRect toRect(const QRectF& r) { return {(int)r.x(), (int)r.y(), (int)r.width(), (int)r.height()}; }
const QRect& toRect(const QRect& r) { return r; }

void ShapeCornersEffect::prePaintWindow(KWin::EffectWindow *w, KWin::WindowPrePaintData &data, std::chrono::milliseconds time)
{
    if (!hasEffect(w))
    {
        Effect::prePaintWindow(w, data, time);
        return;
    }

    auto size = isWindowActive(w) ? ShapeCornersConfig::size(): ShapeCornersConfig::inactiveCornerRadius();

#if KWIN_EFFECT_API_VERSION >= 234
    const auto geo_ex = w->expandedGeometry() * KWin::effects->renderTargetScale();
    const auto geo = w->frameGeometry() * KWin::effects->renderTargetScale();
    data.setTranslucent();
#else
    const auto& geo_ex = w->expandedGeometry();
    const auto& geo = w->expandedGeometry();
#endif
    QRegion reg {};
    reg += toRect(geo_ex);
    reg -= toRect(geo);
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
    m_shaderManager.Bind(w, isTiled(w));
    glActiveTexture(GL_TEXTURE0);

#if KWIN_EFFECT_API_VERSION >= 236
    OffscreenEffect::drawWindow(w, mask, region, data);
#else
    DeformEffect::drawWindow(w, mask, region, data);
#endif
    m_shaderManager.Unbind();
}

bool ShapeCornersEffect::hasEffect(const KWin::EffectWindow *w) const {
    auto name = w->windowClass().split(' ').first();
    return m_shaderManager.IsValid()
           && m_managed.contains(w)
           && (w->hasDecoration() || ShapeCornersConfig::inclusions().contains(name))
           && !ShapeCornersConfig::exclusions().contains(name);
}

QString ShapeCornersEffect::get_window_titles() {
    QList<QString> response;
    for (const auto& [win, tiled]: m_managed) {
        auto name = win->windowClass().split(' ').first();
        if (name == "plasmashell")
            continue;
        if (!response.contains(name))
            response.push_back(name);
    }
    return response.join("\n");
}

bool ShapeCornersEffect::checkTiled(const bool& horizontal, double window_start, double gap, const double& screen_size) {
    if (window_start + gap == screen_size)
        return true;

    bool firstGap = gap == -1;
    #define DIM(a,b) (a*horizontal + b*!horizontal)

    bool r = false;
    for (auto& [w, tiled]: m_managed) {
        if (firstGap)
            gap = DIM(w->x(), w->y()) - window_start;

        if (DIM(w->x(), w->y()) == window_start + gap) {
            if (checkTiled(horizontal, window_start + DIM(w->width(), w->height()), gap, screen_size)) {
                tiled = true;
                r = true;
            }
        }
    }
    return r;
}

void ShapeCornersEffect::checkTiled() {
    if (!ShapeCornersConfig::disableRoundTile() && !ShapeCornersConfig::disableOutlineTile())
        return;

    for (auto& [w, tiled]: m_managed) {
        tiled = false;
    }
    for (const auto& screen: KWin::effects->screens()) {
        const auto& geometry = screen->geometry();
        checkTiled(true, geometry.x(), -1, geometry.width());
        checkTiled(false, geometry.y(), -1, geometry.height());
    }
}
