
#include "Window.h"
#include <effect/effecthandler.h>
#include "Config.h"
#include "WindowConfig.h"

ShapeCorners::Window::Window(KWin::EffectWindow *kwindow) :
    w(kwindow), lastAnimationDuration(Config::animationDuration()), currentConfig(WindowConfig::inactiveWindowConfig())
{
    connect(Config::self(), &Config::configChanged, this, &Window::configChanged);
    configChanged();
}

bool ShapeCorners::Window::isActive() const { return KWin::effects->activeWindow() == w; }

bool ShapeCorners::Window::hasEffect(const bool isPainting) const
{
    if (!w->expandedGeometry().isValid()) {
        return false;
    }
    if (!(w->isNormalWindow() && Config::includeNormalWindows()) && !(w->isDialog() && Config::includeDialogs()) &&
        !isIncluded) {
        return false;
    }
    if (isExcluded) {
        return false;
    }
    if (!hasRoundCorners() && !hasOutline()) {
        return false;
    }
    // Paint passes keep windows KWin still draws while they animate out, such as a window being minimized.
    if (!isPainting && !w->isVisible()) {
        return false;
    }
    // Outside paint passes, windows hidden by Show Desktop have no effect.
    if (!isPainting && w->isHiddenByShowDesktop()) {
        return false;
    }
    return true;
}

bool ShapeCorners::Window::hasRoundCorners() const
{
    if (currentConfig.cornerRadius <= 0) {
        return false;
    }
    if (w->isFullScreen()) {
        return !Config::disableRoundFullScreen();
    }
    if (isMaximized) {
        return !Config::disableRoundMaximize();
    }
    if (isTiled) {
        return !Config::disableRoundTile();
    }
    return true;
}

bool ShapeCorners::Window::hasOutline() const
{
    if (w->isFullScreen()) {
        return !Config::disableOutlineFullScreen();
    }
    if (isMaximized) {
        return !Config::disableOutlineMaximize();
    }
    if (isTiled) {
        return !Config::disableOutlineTile();
    }
    return true;
}

#ifdef QT_DEBUG
QDebug KWin::operator<<(const QDebug &debug, const EffectWindow &kwindow)
{
    return (debug << kwindow.windowType() << kwindow.windowClass() << kwindow.caption());
}
#endif

void ShapeCorners::Window::configChanged()
{
    isExcluded = false;
    isIncluded = false;
    for (const auto &exclusion: Config::exclusions()) {
        if (w->windowClass().contains(exclusion, Qt::CaseInsensitive) ||
            captionAfterDash().contains(exclusion, Qt::CaseInsensitive)) {
            isExcluded = true;
#ifdef DEBUG_INCLUSIONS
            qDebug() << "ShapeCorners: Excluded window:" << *this;
#endif
            return;
        }
    }
    for (const auto &inclusion: Config::inclusions()) {
        if (w->windowClass().contains(inclusion, Qt::CaseInsensitive) ||
            captionAfterDash().contains(inclusion, Qt::CaseInsensitive)) {
            isIncluded = true;
#ifdef DEBUG_INCLUSIONS
            qDebug() << "ShapeCorners: Included window:" << *this;
#endif
            return;
        }
    }
}

QJsonObject ShapeCorners::Window::toJson() const
{
    QJsonObject json;
    json.insert(QStringLiteral("class"), w->windowClass());
    json.insert(QStringLiteral("caption"), w->caption());
    return json;
}

QString ShapeCorners::Window::captionAfterDash() const
{
    const auto sep   = QStringLiteral(" — ");
    const auto index = w->caption().indexOf(sep);
    if (index == -1) {
        return w->caption();
    }
    return w->caption().mid(index + sep.size());
}
