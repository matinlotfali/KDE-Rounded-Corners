//
// Created by matin on 10/04/24.
//

#include "Window.h"
#include "Config.h"

#if QT_VERSION_MAJOR >= 6
#include <effect/effecthandler.h>
#else
#include <kwineffects.h>
#endif

inline static float clamp(const float value, const float delta, const float config) {
    if (delta > 0 && value > config)
        return config;
    if (delta < 0 && value < 0)
        return 0;
    return value;
}

QWidget ShapeCorners::Window::m_widget {};

ShapeCorners::Window::Window(KWin::EffectWindow& w)
    : w(w)
    , cornerRadius(static_cast<float>(Config::inactiveCornerRadius()))
{
    configChanged();
}

bool ShapeCorners::Window::isActive() const {
    return KWin::effects->activeWindow() == &w;
}

bool ShapeCorners::Window::hasEffect() const {
    return (
            w.expandedGeometry().isValid()
            && (
                    (w.isNormalWindow() && Config::includeNormalWindows())
                    || (w.isDialog() && Config::includeDialogs())
                    || isIncluded
            )
            && !isExcluded
            && (hasRoundCorners() || hasOutline())
    );
}

bool ShapeCorners::Window::hasRoundCorners() const {
    if (cornerRadius <= 0)
        return false;
    if (w.isFullScreen())
        return !Config::disableRoundFullScreen();
    if (isMaximized)
        return !Config::disableRoundMaximize();
    if (isTiled)
        return !Config::disableRoundTile();
    return true;
}

bool ShapeCorners::Window::hasOutline() const {
    if (w.isFullScreen())
        return !Config::disableOutlineFullScreen();
    if (isMaximized)
        return !Config::disableOutlineMaximize();
    if (isTiled)
        return !Config::disableOutlineTile();
    return true;
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

    const auto deltaTime = static_cast<float>((time - m_last_time).count());
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
    deltaCornerRadius = std::round(deltaCornerRadius * 100) / 100.f;
    deltaShadowSize = std::round(deltaShadowSize * 100) / 100.f;
    deltaOutlineSize = std::round(deltaOutlineSize * 100) / 100.f;
    deltaSecondOutlineSize = std::round(deltaSecondOutlineSize * 100) / 100.f;
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
                qDebug() << "ShapeCorners: repainted" << w.windowClass() << repaintCount << "times for animation.";
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
    cornerRadius = clamp(cornerRadius, deltaCornerRadius, configCornerRadius);
    shadowSize = clamp(shadowSize, deltaShadowSize, configShadowSize);
    outlineSize = clamp(outlineSize, deltaOutlineSize, configOutlineSize);
    secondOutlineSize = clamp(secondOutlineSize, deltaSecondOutlineSize, configSecondOutlineSize);
    shadowColor.clamp();
    outlineColor.clamp();
    secondOutlineColor.clamp();

    // the animation is still in progress
#ifdef DEBUG_ANIMATION
    repaintCount++;
#endif
    w.addRepaintFull();
}

#ifdef QT_DEBUG
QDebug KWin::operator<<(QDebug& debug, const EffectWindow& w) {
    return (debug << w.windowType() << w.windowClass() << w.caption());
}
#endif

void ShapeCorners::Window::configChanged() {
    isExcluded = false;
    isIncluded = false;
    for (auto& exclusion: Config::exclusions()) {
        if (w.windowClass().contains(exclusion, Qt::CaseInsensitive)
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
        if (w.windowClass().contains(inclusion, Qt::CaseInsensitive)
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

QJsonObject ShapeCorners::Window::toJson() const {
    auto json = QJsonObject();
    json[QStringLiteral("class")] = w.windowClass();
    json[QStringLiteral("caption")] = w.caption();
    return json;
}

QString ShapeCorners::Window::captionAfterDash() const {
    const auto sep = QStringLiteral(" — ");
    const auto index = w.caption().indexOf(sep);
    if (index == -1)
        return w.caption();
    return w.caption().mid(index + sep.size());
}
