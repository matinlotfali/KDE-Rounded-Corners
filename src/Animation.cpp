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
    if (const auto active = KWin::effects->activeWindow();
        (active && !currentActiveWindow) || (currentActiveWindow && active != currentActiveWindow->w)) {
        const auto activeWindow = WindowManager::instance()->findWindow(active);
        setActiveWindowChanged(activeWindow);
    }

    // If not animating, skip update.
    if (!window.isAnimating) {
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
        window.currentConfig =
                window.isActive() ? WindowConfig::activeWindowConfig() : WindowConfig::inactiveWindowConfig();
        return;
    }

    // Interpolate between active and inactive configs based on animation progress.
    const auto deltaTime  = static_cast<float>(window.lastAnimationDuration) / static_cast<float>(animationDuration);
    const auto configDiff = WindowConfig::activeWindowConfig() - WindowConfig::inactiveWindowConfig();
    const auto deltaConf  = configDiff * deltaTime;

    if (window.isActive()) {
        window.currentConfig = WindowConfig::activeWindowConfig() - deltaConf;
    } else {
        window.currentConfig = WindowConfig::inactiveWindowConfig() + deltaConf;
    }
    window.isAnimating = true;
}

void ShapeCorners::Animation::setActiveWindowChanged(Window *window)
{
#ifdef QT_DEBUG
    if (window) {
        qDebug() << "ShapeCorners: Window activated" << window;
    } else {
        qDebug() << "ShapeCorners: No window activated";
    }
#endif

    lastActiveWindow    = currentActiveWindow;
    currentActiveWindow = window;

    // If not animating, start a new animation cycle for the newly activated window.
    if (currentActiveWindow && !currentActiveWindow->isAnimating) {
        currentActiveWindow->lastActiveChangedTime = system_clock::now();
        currentActiveWindow->lastAnimationDuration = Config::animationDuration();
        currentActiveWindow->isAnimating           = currentActiveWindow->lastAnimationDuration > 0;
    }

    // If not animating, start a new animation cycle for the non-activated window.
    if (lastActiveWindow && !lastActiveWindow->isAnimating) {
        lastActiveWindow->lastActiveChangedTime = system_clock::now();
        lastActiveWindow->lastAnimationDuration = Config::animationDuration();
        lastActiveWindow->isAnimating           = lastActiveWindow->lastAnimationDuration > 0;
    }
}
