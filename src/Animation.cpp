//
// Created by matin on 8/3/25.
//

#include "Animation.h"
#include <QDebug>
#if QT_VERSION_MAJOR >= 6
#include <effect/effecthandler.h>
#else
#include <kwineffects.h>
#endif

#include "Config.h"
#include "Window.h"
#include "WindowConfig.h"

using namespace std::chrono;

ShapeCorners::Animation::Animation() : lastAnimationDuration(Config::animationDuration())
{
    // Initialize animation state.
    update();
}

void ShapeCorners::Animation::update()
{
    if (const auto active = KWin::effects->activeWindow(); active != lastActiveWindow) {
        setActiveWindowChanged(active);
        lastActiveWindow = active;
    }

    // If not animating, skip update.
    if (!m_isAnimating) {
        return;
    }

    // If animation duration is zero, stop animating and set configs to final state.
    if (lastAnimationDuration == 0) {
        m_isAnimating = false;
        qDebug() << "ShapeCorners: Animation ended.";
        return;
    }

    // Calculate remaining animation duration.
    const auto now               = system_clock::now();
    const auto animationDuration = Config::animationDuration();
    lastAnimationDuration = animationDuration + duration_cast<milliseconds>(lastActiveWindowChangedTime - now).count();

    // If animation is over, set configs to their final values.
    if (lastAnimationDuration < 0) {
        lastAnimationDuration = 0;
        activeAnimation       = WindowConfig::activeWindowConfig();
        inactiveAnimation     = WindowConfig::inactiveWindowConfig();
        return;
    }

    // Interpolate between active and inactive configs based on animation progress.
    const auto deltaTime  = static_cast<float>(lastAnimationDuration) / static_cast<float>(animationDuration);
    const auto configDiff = WindowConfig::activeWindowConfig() - WindowConfig::inactiveWindowConfig();
    const auto deltaConf  = configDiff * deltaTime;

    activeAnimation   = WindowConfig::activeWindowConfig() - deltaConf;
    inactiveAnimation = WindowConfig::inactiveWindowConfig() + deltaConf;
    m_isAnimating     = true;
}

const ShapeCorners::WindowConfig *ShapeCorners::Animation::getFrameConfig(const Window &window) const
{
    // Return the appropriate config based on window activity.
    return window.isActive() ? &activeAnimation : &inactiveAnimation;
}

void ShapeCorners::Animation::setActiveWindowChanged(const KWin::EffectWindow *w)
{
#ifdef QT_DEBUG
    if (w) {
        qDebug() << "ShapeCorners: Window activated" << *w;
    } else {
        qDebug() << "ShapeCorners: No window activated";
    }
#else
    Q_UNUSED(w)
#endif

    // If not animating, start a new animation cycle.
    if (!m_isAnimating) {
        lastActiveWindowChangedTime = system_clock::now();
        lastAnimationDuration       = Config::animationDuration();
        m_isAnimating               = lastAnimationDuration > 0;

        // If animation is disabled, set configs to their final values.
        if (!m_isAnimating) {
            activeAnimation   = WindowConfig::activeWindowConfig();
            inactiveAnimation = WindowConfig::inactiveWindowConfig();
        }
    }
}
