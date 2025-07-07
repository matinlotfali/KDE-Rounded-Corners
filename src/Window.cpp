
#include "Window.h"
#include "Config.h"
#if QT_VERSION_MAJOR >= 6
#include <effect/effecthandler.h>
#else
#include <kwineffects.h>
#endif

ShapeCorners::Window::Window(KWin::EffectWindow* kwindow)
    : w(kwindow)
    , currentConfig(getExpectedConfig())
{
    connect(Config::self(), &Config::configChanged, this, &Window::configChanged);
    configChanged();
}

bool ShapeCorners::Window::isActive() const noexcept {
    return KWin::effects->activeWindow() == w;
}

bool ShapeCorners::Window::hasEffect() const noexcept {
    return (
        w->expandedGeometry().isValid()
        && (
                (w->isNormalWindow() && Config::includeNormalWindows())
                || (w->isDialog() && Config::includeDialogs())
                || isIncluded
        )
        && !isExcluded
        && (hasRoundCorners() || hasOutline())
    );
}

bool ShapeCorners::Window::hasRoundCorners() const noexcept {
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

bool ShapeCorners::Window::hasOutline() const noexcept {
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

ShapeCorners::WindowConfig ShapeCorners::Window::getExpectedConfig() const noexcept {

    // Choose active or inactive config values based on window state
    WindowConfig config = isActive() ?
        WindowConfig::activeWindowConfig():
        WindowConfig::inactiveWindowConfig();
    if (!hasOutline()) {
        config.outlineColor.setAlpha(0);
        config.secondOutlineColor.setAlpha(0);
    }
    return config;
}

void ShapeCorners::Window::animateProperties(const std::chrono::milliseconds& time) noexcept {

    // If the properties are not initialized yet, don't animate them.
    if (!Config::animationEnabled()) {
        currentConfig = getExpectedConfig();
        return;
    }

    // Calculate the time delta since the last animation step
    const auto deltaTime = static_cast<float>((time - m_last_time).count());
    m_last_time = time;
    if (deltaTime <= 0) {
        return;
    }

    // Find the destination config
    const auto destinationConfig = getExpectedConfig();

    // Calculate the animation step
    auto deltaConfig = (destinationConfig - currentConfig) / deltaTime;

    // Adjust decimal precision for smoother animation
    deltaConfig.round();

    // If all deltas are zero, the animation is finished
    if (!deltaConfig) {
#ifdef DEBUG_ANIMATION
        if (repaintCount > 0) {
            qDebug() << "ShapeCorners: repainted" << w.windowClass() << repaintCount << "times for animation.";
        }
        repaintCount = 0;
#endif
        return;
    }

    // Adjust properties by their deltas
    currentConfig += deltaConfig;

    // Clamp properties to their target values and valid ranges
    currentConfig.clamp(deltaConfig, destinationConfig);

    // The animation is still in progress
#ifdef DEBUG_ANIMATION
    repaintCount++;
#endif
    w->addRepaintFull();
}

#ifdef QT_DEBUG
QDebug KWin::operator<<(const QDebug & debug, EffectWindow& kwindow) {
    return (debug << kwindow.windowType() << kwindow.windowClass() << kwindow.caption());
}
#endif

void ShapeCorners::Window::configChanged() {
    isExcluded = false;
    isIncluded = false;
    for (auto& exclusion: Config::exclusions()) {
        if (w->windowClass().contains(exclusion, Qt::CaseInsensitive)
            || captionAfterDash().contains(exclusion, Qt::CaseInsensitive)
                ) {
            isExcluded = true;
#ifdef DEBUG_INCLUSIONS
            qDebug() << "ShapeCorners: Excluded window:" << *this;
#endif
            return;
        }
    }
    for (auto& inclusion: Config::inclusions()) {
        if (w->windowClass().contains(inclusion, Qt::CaseInsensitive)
            || captionAfterDash().contains(inclusion, Qt::CaseInsensitive)
        ) {
            isIncluded = true;
#ifdef DEBUG_INCLUSIONS
            qDebug() << "ShapeCorners: Included window:" << *this;
#endif
            return;
        }
    }
}

QJsonObject ShapeCorners::Window::toJson() const noexcept {
    auto json = QJsonObject();
    json[QStringLiteral("class")] = w->windowClass();
    json[QStringLiteral("caption")] = w->caption();
    return json;
}

QString ShapeCorners::Window::captionAfterDash() const noexcept {
    const auto sep = QStringLiteral(" — ");
    const auto index = w->caption().indexOf(sep);
    if (index == -1) {
        return w->caption();
    }
    return w->caption().mid(index + sep.size());
}
