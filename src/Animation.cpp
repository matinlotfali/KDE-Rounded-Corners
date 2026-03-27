//
// Created by matin on 8/3/25.
//

#include "Animation.h"
#include <QDebug>

#include "WindowManager.h"
#if QT_VERSION_MAJOR >= 6
#include <effect/effecthandler.h>
#else
#include <kwineffects.h>
#endif

#include "Config.h"
#include "Window.h"
#include "WindowConfig.h"

using namespace std::chrono;

ShapeCorners::Animation::Animation() = default;

void ShapeCorners::Animation::update(Window &window)
{
    // If the active window has changed, start a new animation cycle.
    if (auto *const active = KWin::effects->activeWindow();
        ((active != nullptr) && (currentActiveWindow == nullptr)) ||
        ((currentActiveWindow != nullptr) && active != currentActiveWindow->w)) {
        auto *const activeWindow = WindowManager::instance()->findWindow(active);
        setActiveWindowChanged(activeWindow);
    }

    // Fetch the current active and inactive configs from the palette once per update.
    const auto activeConfig   = WindowConfig::activeWindowConfig();
    const auto inactiveConfig = WindowConfig::inactiveWindowConfig();

    // If not animating, skip update.
    if (!window.isAnimating) {
        window.currentConfig = window.isActive() ? activeConfig : inactiveConfig;
        return;
    }

    // If animation duration is zero, stop animating and set configs to final state.
    if (window.lastAnimationDuration == 0) {
        window.isAnimating = false;
        qDebug() << "ShapeCorners: Animation ended.";
        return;
    }

    // Calculate remaining animation duration.
    const auto now               = system_clock::now();
    const auto animationDuration = Config::animationDuration();
    window.lastAnimationDuration =
            animationDuration + duration_cast<milliseconds>(window.lastActiveChangedTime - now).count();

    // If animation is over, set configs to their final values.
    if (window.lastAnimationDuration < 0) {
        window.lastAnimationDuration = 0;
        window.currentConfig = window.isActive() ? activeConfig : inactiveConfig;
        return;
    }

    // Interpolate between active and inactive configs based on animation progress.
    const auto deltaTime  = static_cast<float>(window.lastAnimationDuration) / static_cast<float>(animationDuration);
    const auto configDiff = activeConfig - inactiveConfig;
    const auto deltaConf  = configDiff * deltaTime;

    if (window.isActive()) {
        window.currentConfig = activeConfig - deltaConf;
    } else {
        window.currentConfig = inactiveConfig + deltaConf;
    }
    window.isAnimating = true;
}

void ShapeCorners::Animation::setActiveWindowChanged(Window *window)
{
#ifdef QT_DEBUG
    if (window != nullptr) {
        qDebug() << "ShapeCorners: Window activated" << *window;
    } else {
        qDebug() << "ShapeCorners: No window activated";
    }
#endif

    lastActiveWindow    = currentActiveWindow;
    currentActiveWindow = window;

    // If animation is disabled, just set the config and skip animation settings
    if (Config::animationDuration() == 0) {
        if (currentActiveWindow != nullptr) {
            currentActiveWindow->currentConfig = WindowConfig::activeWindowConfig();
        }
        if (lastActiveWindow != nullptr) {
            lastActiveWindow->currentConfig = WindowConfig::inactiveWindowConfig();
        }
        return;
    }

    // If not animating, start a new animation cycle for the newly activated window.
    if ((currentActiveWindow != nullptr) && !currentActiveWindow->isAnimating) {
        currentActiveWindow->lastActiveChangedTime = system_clock::now();
        currentActiveWindow->lastAnimationDuration = Config::animationDuration();
        currentActiveWindow->isAnimating           = currentActiveWindow->lastAnimationDuration > 0;
    }

    // If not animating, start a new animation cycle for the non-activated window.
    if ((lastActiveWindow != nullptr) && !lastActiveWindow->isAnimating) {
        lastActiveWindow->lastActiveChangedTime = system_clock::now();
        lastActiveWindow->lastAnimationDuration = Config::animationDuration();
        lastActiveWindow->isAnimating           = lastActiveWindow->lastAnimationDuration > 0;
    }
}
