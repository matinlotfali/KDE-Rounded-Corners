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
#include "ShapeCornersConfig.h"
#include <QtDBus/QDBusConnection>
#include <QDBusError>
#include <KX11Extras>

#if QT_VERSION_MAJOR >= 6
    #include <opengl/glutils.h>
    #include <effect/effecthandler.h>
    #include <core/output.h>
    #include <core/renderviewport.h>
#else
    #include <kwinglutils.h>
#endif

ShapeCornersEffect::ShapeCornersEffect()
    : KWin::OffscreenEffect()
{
    reconfigure(ReconfigureAll);

    if (auto connection = QDBusConnection::sessionBus();
        !connection.isConnected()) {
        qWarning("ShapeCorners: Cannot connect to the D-Bus session bus.\n");
    }
    else {
        if (!connection.registerService(QStringLiteral("org.kde.ShapeCorners"))) {
            qWarning("%s\n", qPrintable(connection.lastError().message()));
        }
        else {
            if (!connection.registerObject(QStringLiteral("/ShapeCornersEffect"), this, QDBusConnection::ExportAllSlots)) {
                qWarning("%s\n", qPrintable(connection.lastError().message()));
            }
        }
    }

    if(m_shaderManager.IsValid()) {
        for (const auto& win: KWin::effects->stackingOrder())
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
    const auto& screenGeometry = KWin::effects->findScreen(w->screen()->name())->geometry();
    return (w->x() == screenGeometry.x() && w->width() == screenGeometry.width()) ||
           (w->y() == screenGeometry.y() && w->height() == screenGeometry.height());
}

void ShapeCornersEffect::prePaintWindow(KWin::EffectWindow *w, KWin::WindowPrePaintData &data, std::chrono::milliseconds time)
{
    if (!hasEffect(w))
    {
        Effect::prePaintWindow(w, data, time);
        return;
    }

    const auto size = isWindowActive(w) ? ShapeCornersConfig::size(): ShapeCornersConfig::inactiveCornerRadius();

#if QT_VERSION_MAJOR >= 6
    const auto geo = w->frameGeometry() * w->screen()->scale();
    data.setTranslucent();
#else
    const auto geo = w->frameGeometry() * KWin::effects->renderTargetScale();
    data.setTranslucent();
#endif
    QRegion reg {};
    reg += QRect(geo.x(), geo.y(), size, size);
    reg += QRect(geo.x()+geo.width()-size, geo.y(), size, size);
    reg += QRect(geo.x(), geo.y()+geo.height()-size, size, size);
    reg += QRect(geo.x()+geo.width()-size, geo.y()+geo.height()-size, size, size);
    data.opaque -= reg;
    data.paint += reg;

    OffscreenEffect::prePaintWindow(w, data, time);
}

bool ShapeCornersEffect::supported()
{
    return KWin::effects->isOpenGLCompositing();
}

#if QT_VERSION_MAJOR >= 6
void ShapeCornersEffect::drawWindow(const KWin::RenderTarget &renderTarget, const KWin::RenderViewport &viewport,
                                    KWin::EffectWindow *w, int mask, const QRegion &region,
                                    KWin::WindowPaintData &data) {
#else
void ShapeCornersEffect::drawWindow(KWin::EffectWindow *w, int mask, const QRegion &region,
                                    KWin::WindowPaintData &data) {
#endif
    if (!hasEffect(w))
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
    m_shaderManager.Bind(w, scale);
    glActiveTexture(GL_TEXTURE0);

#if QT_VERSION_MAJOR >= 6
    OffscreenEffect::drawWindow(renderTarget, viewport, w, mask, region, data);
#else
    OffscreenEffect::drawWindow(w, mask, region, data);
#endif
    m_shaderManager.Unbind();
}

bool ShapeCornersEffect::hasEffect(const KWin::EffectWindow *w) const {
    const auto name = w->windowClass().split(QChar::Space).first();
    return m_shaderManager.IsValid()
           && m_managed.contains(w)
           && (w->isNormalWindow() || ShapeCornersConfig::inclusions().contains(name))
           && !ShapeCornersConfig::exclusions().contains(name)
           && !(ShapeCornersConfig::excludeMaximizedWindows() && isMaximized(w));
}

QString ShapeCornersEffect::get_window_titles() const {
    QSet<QString> response;
    for (const auto& win: m_managed) {
        const auto name = win->windowClass().split(QChar::Space).first();
        if (name == QStringLiteral("plasmashell"))
            continue;
        response.insert(name);
    }
    return response.values().join(QStringLiteral("\n"));
}
