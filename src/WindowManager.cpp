#include "WindowManager.h"
#include "TileChecker.h"
#include "Window.h"
#include "Config.h"
#include <QJsonArray>
#include <QtDBus/QDBusConnection>
#include <QDBusError>
#include <ranges>
#if QT_VERSION_MAJOR >= 6
#include <effect/effectwindow.h>
#include <effect/effecthandler.h>
#else
#include <kwineffects.h>
#endif

ShapeCorners::WindowManager::WindowManager() {
    // Register D-Bus service and object for external communication
    registerDBus();
    
    // Add all currently stacked windows
    for (const auto &kwindow: KWin::effects->stackingOrder()) {
        addWindow(kwindow);
    }
    
    // Listen for window deletion events
    connect(KWin::effects, &KWin::EffectsHandler::windowDeleted, this, &WindowManager::windowRemoved);
}

ShapeCorners::Window *ShapeCorners::WindowManager::findWindow(const KWin::EffectWindow *kwindow) {
    const auto iterator = m_managed.find(kwindow);
    if (iterator != m_managed.end()) {
        return iterator->second;
    }
    return nullptr;
}

bool ShapeCorners::WindowManager::addWindow(KWin::EffectWindow *kwindow)
{
    // Don't treat docks as windows. They are needed for the maximized check only.
    if (kwindow->isDock()) {
#ifdef QT_DEBUG
        qInfo() << "ShapeCorners: menu added." << *kwindow;
#endif
        m_menuBars.push_back(kwindow);
        return false;
    }

#ifdef QT_DEBUG
    qInfo() << "ShapeCorners: window added." << *kwindow;
#endif

    // Ignore windows without a valid class or caption
    if (kwindow->windowClass().trimmed().isEmpty() && kwindow->caption().trimmed().isEmpty()) {
#ifdef QT_DEBUG
        qWarning() << "ShapeCorners: window does not have a valid class name.";
#endif
        return false;
    }

    // Ignore hardcoded exceptions (KWin, lockscreen, etc.)
    const QSet hardExceptions {
        QStringLiteral("kwin"),
        QStringLiteral("kwin_x11"),
        QStringLiteral("kwin_wayland"),
        QStringLiteral("kscreenlocker_greet"),
        QStringLiteral("ksmserver"),
        QStringLiteral("krunner"),
        QStringLiteral("ksplashqml"),
        // QStringLiteral("plasmashell"),        Note: Don't add it to exceptions, it involves widget config windows
    };
    const auto name = kwindow->windowClass().split(QChar::Space).first();
    if (hardExceptions.contains(name)) {
#ifdef QT_DEBUG
        qWarning() << "ShapeCorners: ignoring window explicitly.";
#endif
        return false;
    }

    // Create and add the managed window
    auto* window = new Window(kwindow);
    const auto& [iter, r] = m_managed.emplace(kwindow, window);
    if (!r) {
#ifdef QT_DEBUG
        qWarning() << "ShapeCorners: ignoring duplicate window.";
#endif
        return false;
    }

    // Connect geometry and config change signals
#if QT_VERSION_MAJOR >= 6
    connect(kwindow, &KWin::EffectWindow::windowFrameGeometryChanged, this, &WindowManager::windowResized);
#else
    connect(KWin::effects, &KWin::EffectsHandler::windowFrameGeometryChanged, this, &WindowManager::windowResized);
#endif
    
    // Update tiling and maximized state for the new window
    windowResized(kwindow, QRectF());
    return true;
}

void ShapeCorners::WindowManager::windowRemoved(KWin::EffectWindow *kwindow)
{
    if (kwindow == nullptr) {
        qDebug() << "ShapeCorners: null window removed";
    }
    else if (const auto iterator = m_managed.find(kwindow); iterator != m_managed.end()) {
        iterator->second->deleteLater();
        m_managed.erase(iterator);
        qDebug() << "ShapeCorners: window removed" << kwindow->windowClass();
    }
    else if (const auto iterator2 = std::ranges::find(m_menuBars, kwindow); iterator2 != m_menuBars.end()) {
        m_menuBars.erase(iterator2);
        qDebug() << "ShapeCorners: menu removed" << kwindow->windowClass();
    }
    else {
        qDebug() << "ShapeCorners: excluded window removed" << kwindow->windowClass();
    }

    checkTiled();
}

QString ShapeCorners::WindowManager::get_window_titles() const {
    QJsonArray array;
    for (const auto &window: m_managed | std::views::values) {
        auto json = window->toJson();
        if (!array.contains(json)) {
            array.push_back(json);
        }
    }
    const auto doc = QJsonDocument(array).toJson(QJsonDocument::Compact);
    return QString::fromUtf8(doc);
}

void ShapeCorners::WindowManager::registerDBus() {
    auto connection = QDBusConnection::sessionBus();
    if (!connection.isConnected()) {
        qWarning() << "ShapeCorners: Cannot connect to the D-Bus session bus.";
        return;
    }
    if (!connection.registerService(QStringLiteral("org.kde.ShapeCorners"))) {
        qWarning() << "ShapeCorners: " << connection.lastError().message();
        return;
    }
    if (!connection.registerObject(QStringLiteral("/ShapeCornersEffect"), this, QDBusConnection::ExportAllSlots)) {
        qWarning() << "ShapeCorners: " << connection.lastError().message();
        return;
    }
}

void ShapeCorners::WindowManager::checkTiled() {
    TileChecker tileChecker (m_managed);
    tileChecker.clearTiles();

    // If both tile options are disabled, skip checking
    if (!Config::disableRoundTile() && !Config::disableOutlineTile()) {
        return;
    }

    // Check tiling for each screen, excluding menu bars
    for (const auto& screen: KWin::effects->screens()) {
        const auto screen_region = getRegionWithoutMenus(screen->geometry());
        const auto geometry = screen_region.boundingRect();
        tileChecker.checkTiles(geometry);
    }
}

QRegion ShapeCorners::WindowManager::getRegionWithoutMenus(const QRect& screen_geometry) const {
    auto screen_region = QRegion(screen_geometry);
#ifdef DEBUG_MAXIMIZED
    qDebug() << "ShapeCorners: screen region" << screen_region;
#endif

    // Subtract all menu bar geometries from the screen region
    for (const auto &ptr: m_menuBars) {
#ifdef DEBUG_MAXIMIZED
        qDebug() << "ShapeCorners: menu is" << ptr->frameGeometry();
#endif
        screen_region -= ptr->frameGeometry().toRect();
    }

#ifdef DEBUG_MAXIMIZED
    qDebug() << "ShapeCorners: screen region without menus" << screen_region;
#endif

    return screen_region;
}

void ShapeCorners::WindowManager::checkMaximized(KWin::EffectWindow *kwindow) {
    auto* const window = findWindow(kwindow);
    if (window == nullptr) {
        return;
    }

    window->isMaximized = false;

    const auto screen_region = getRegionWithoutMenus(kwindow->screen()->geometry());

    // Check if the window fills the screen region
    auto remaining = screen_region - kwindow->frameGeometry().toRect();
#ifdef DEBUG_MAXIMIZED
    qDebug() << "ShapeCorners: active window remaining region" << remaining;
#endif
    if (remaining.isEmpty()) {
        window->isMaximized = true;
#ifdef DEBUG_MAXIMIZED
        qInfo() << "ShapeCorners: window maximized" << kwindow->windowClass();
#endif
    }
}

void ShapeCorners::WindowManager::windowResized(KWin::EffectWindow *kwindow, const QRectF &size)
{
    Q_UNUSED(size);

    checkTiled();
    checkMaximized(kwindow);
}