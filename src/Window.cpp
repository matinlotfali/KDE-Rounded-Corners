//
// Created by matin on 10/04/24.
//

#include "Window.h"
#include "Config.h"

#include <qconfig.h>
#if QT_VERSION_MAJOR >= 6
#include <effect/effecthandler.h>
#else
#include <kwineffects.h>
#endif

QWidget ShapeCorners::Window::m_widget {};

ShapeCorners::Window::Window(KWin::EffectWindow *w)
        : w(w), isIncluded(false), isExcluded(false)
{
    configChanged();
}

bool ShapeCorners::Window::isActive() const {
    return KWin::effects->activeWindow() == w;
}

bool ShapeCorners::Window::hasEffect() const {
    return (
            (
                    (w->isNormalWindow() && Config::includeNormalWindows())
                    || (w->isDialog() && Config::includeDialogs())
                    || isIncluded
            )
            && !isExcluded
            && (hasRoundCorners() || hasOutline())
    );
}

bool ShapeCorners::Window::hasRoundCorners() const {
    return !(isTiled && Config::disableRoundTile() && !isMaximized)
           && !(isMaximized && Config::disableRoundMaximize());
}

bool ShapeCorners::Window::hasOutline() const {
    return !(isTiled && Config::disableOutlineTile() && !isMaximized)
           && !(isMaximized && Config::disableOutlineMaximize());
}

constexpr void clamp(float& value, const float& delta, const float& config) {
    if (delta > 0)
        value = std::min(value, config);
    else if (delta < 0)
        value = std::max(value, 0.0f);
}

void ShapeCorners::Window::animateProperties(const std::chrono::milliseconds& time) {

    // find the destination value
    float configCornerRadius;
    float configShadowSize;
    float configOutlineSize;
    float configSecondOutlineSize;
    Color configShadowColor;
    Color configOutlineColor;
    Color configSecondOutlineColor;
    const QPalette& m_palette = m_widget.palette();
    if (isActive()) {
        configCornerRadius = static_cast<float>(Config::size());
        configShadowSize = static_cast<float>(Config::shadowSize());
        configOutlineSize = static_cast<float>(Config::outlineThickness());
        configSecondOutlineSize = static_cast<float>(Config::secondOutlineThickness());

        configShadowColor = Color(Config::activeShadowUsePalette() ?
                                  m_palette.color(QPalette::Active, static_cast<QPalette::ColorRole>(Config::activeShadowPalette())):
                                  Config::shadowColor());
        configShadowColor.setAlpha(Config::activeShadowAlpha());

        configOutlineColor = Color(Config::activeOutlineUsePalette() ?
                                   m_palette.color(QPalette::Active, static_cast<QPalette::ColorRole>(Config::activeOutlinePalette())):
                                   Config::outlineColor());
        configOutlineColor.setAlpha(hasOutline() ? Config::activeOutlineAlpha(): 0);

        configSecondOutlineColor = Color(Config::activeSecondOutlineUsePalette() ?
                                    m_palette.color(QPalette::Active, static_cast<QPalette::ColorRole>(Config::activeSecondOutlinePalette())):
                                    Config::secondOutlineColor());
        configSecondOutlineColor.setAlpha(hasOutline() ? Config::activeSecondOutlineAlpha(): 0);
    } else {
        configCornerRadius = static_cast<float>(Config::inactiveCornerRadius());
        configShadowSize = static_cast<float>(Config::inactiveShadowSize());
        configOutlineSize = static_cast<float>(Config::inactiveOutlineThickness());
        configSecondOutlineSize = static_cast<float>(Config::inactiveSecondOutlineThickness());

        configShadowColor = Color(Config::inactiveShadowUsePalette() ?
                                  m_palette.color(QPalette::Inactive, static_cast<QPalette::ColorRole>(Config::inactiveShadowPalette())):
                                  Config::inactiveShadowColor());
        configShadowColor.setAlpha(Config::inactiveShadowAlpha());

        configOutlineColor = Color(Config::inactiveOutlineUsePalette() ?
                                   m_palette.color(QPalette::Inactive, static_cast<QPalette::ColorRole>(Config::inactiveOutlinePalette())):
                                   Config::inactiveOutlineColor());
        configOutlineColor.setAlpha(hasOutline() ? Config::inactiveOutlineAlpha(): 0);

        configSecondOutlineColor = Color(Config::inactiveSecondOutlineUsePalette() ?
                                    m_palette.color(QPalette::Inactive, static_cast<QPalette::ColorRole>(Config::inactiveSecondOutlinePalette())):
                                    Config::inactiveSecondOutlineColor());
        configSecondOutlineColor.setAlpha(hasOutline() ? Config::inactiveSecondOutlineAlpha(): 0);
    }

    // if the properties are not initialized yet, don't animate them.
    if (!Config::animationEnabled()
        || cornerRadius == -1
        || shadowSize == -1
        || outlineSize == -1
        || secondOutlineSize == -1
    ) {
        cornerRadius = configCornerRadius;
        shadowSize = configShadowSize;
        outlineSize = configOutlineSize;
        secondOutlineSize = configSecondOutlineSize;
        shadowColor = configShadowColor;
        outlineColor = configOutlineColor;
        secondOutlineColor = configSecondOutlineColor;
        return;
    }

    auto deltaTime = static_cast<float>((time - m_last_time).count());
    m_last_time = time;
    if (deltaTime <= 0)
        return;

    // calculate the animation step
    auto deltaCornerRadius = (configCornerRadius - cornerRadius) / deltaTime;
    auto deltaShadowSize = (configShadowSize - shadowSize) / deltaTime;
    auto deltaOutlineSize = (configOutlineSize - outlineSize) / deltaTime;
    auto deltaSecondOutlineSize = (configSecondOutlineSize - secondOutlineSize) / deltaTime;
    auto deltaShadowColor = (configShadowColor - shadowColor) / deltaTime;
    auto deltaOutlineColor = (configOutlineColor - outlineColor) / deltaTime;
    auto deltaSecondOutlineColor = (configSecondOutlineColor - secondOutlineColor) / deltaTime;

    // adjust decimal precision
    deltaCornerRadius = std::round(deltaCornerRadius * 10) / 10;
    deltaShadowSize = std::round(deltaShadowSize * 10) / 10;
    deltaOutlineSize = std::round(deltaOutlineSize * 10) / 10;
    deltaSecondOutlineSize = std::round(deltaSecondOutlineSize * 10) / 10;
    deltaShadowColor.round();
    deltaOutlineColor.round();
    deltaSecondOutlineColor.round();

    // return false if the animation is over
    if (deltaCornerRadius == 0
        && deltaShadowSize == 0
        && deltaOutlineSize == 0
        && deltaSecondOutlineSize == 0
        && deltaShadowColor.isZero()
        && deltaOutlineColor.isZero()
        && deltaSecondOutlineColor.isZero()
    ) {
#ifdef DEBUG_ANIMATION
        if (repaintCount > 0)
                qDebug() << "ShapeCorners: repainted" << w->windowClass() << repaintCount << "times for animation.";
            repaintCount = 0;
#endif
        return;
    }

    // adjust properties
    cornerRadius += deltaCornerRadius;
    shadowSize += deltaShadowSize;
    outlineSize += deltaOutlineSize;
    secondOutlineSize += deltaSecondOutlineSize;
    shadowColor += deltaShadowColor;
    outlineColor += deltaOutlineColor;
    secondOutlineColor += deltaSecondOutlineColor;

    // check boundaries after adjusting
    clamp(cornerRadius, deltaCornerRadius, configCornerRadius);
    clamp(shadowSize, deltaShadowSize, configShadowSize);
    clamp(outlineSize, deltaOutlineSize, configOutlineSize);
    clamp(secondOutlineSize, deltaSecondOutlineSize, configSecondOutlineSize);
    shadowColor.clamp();
    outlineColor.clamp();
    secondOutlineColor.clamp();

    // the animation is still in progress
#ifdef DEBUG_ANIMATION
    repaintCount++;
#endif
    w->addRepaintFull();
}

QString ShapeCorners::Window::debugName(const KWin::EffectWindow* w) {
    return QStringLiteral("\tclass: ") + w->windowClass() +
#if QT_VERSION_MAJOR >= 6
           QStringLiteral("\ttype: ") + QVariant::fromValue(w->windowType()).toString() +
#else
           QStringLiteral("\ttype: ") + QString::number(static_cast<int>(w->windowType())) +
#endif
           QStringLiteral("\trole: ") + w->windowRole() +
           QStringLiteral("\tcaption: ") + w->caption();
}

void ShapeCorners::Window::configChanged() {
    for (auto& inclusion: Config::inclusions()) {
        if (w->windowClass().contains(inclusion, Qt::CaseInsensitive)
            || w->caption().contains(inclusion, Qt::CaseInsensitive)
                ) {
            isIncluded = true;
            break;
        }
    }
    for (auto& exclusion: Config::exclusions()) {
        if (w->windowClass().contains(exclusion, Qt::CaseInsensitive)
            || w->caption().contains(exclusion, Qt::CaseInsensitive)
                ) {
            isExcluded = true;
            break;
        }
    }
}
