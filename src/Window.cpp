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

ShapeCorners::Window::Window(KWin::EffectWindow& w)
        : w(w), isIncluded(false), isExcluded(false)
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
    float configOutlineSize;
    float configSecondOutlineSize;
    Color configOutlineColor;
    Color configSecondOutlineColor;
    const QPalette& m_palette = m_widget.palette();
    if (isActive()) {
        configCornerRadius = static_cast<float>(Config::size());
        configOutlineSize = static_cast<float>(Config::outlineThickness());
        configSecondOutlineSize = static_cast<float>(Config::secondOutlineThickness());

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
        configOutlineSize = static_cast<float>(Config::inactiveOutlineThickness());
        configSecondOutlineSize = static_cast<float>(Config::inactiveSecondOutlineThickness());

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
        || outlineSize == -1
        || secondOutlineSize == -1
    ) {
        cornerRadius = configCornerRadius;
        outlineSize = configOutlineSize;
        secondOutlineSize = configSecondOutlineSize;
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
    auto deltaOutlineSize = (configOutlineSize - outlineSize) / deltaTime;
    auto deltaSecondOutlineSize = (configSecondOutlineSize - secondOutlineSize) / deltaTime;
    auto deltaOutlineColor = (configOutlineColor - outlineColor) / deltaTime;
    auto deltaSecondOutlineColor = (configSecondOutlineColor - secondOutlineColor) / deltaTime;

    // adjust decimal precision
    deltaCornerRadius = std::round(deltaCornerRadius * 10) / 10;
    deltaOutlineSize = std::round(deltaOutlineSize * 10) / 10;
    deltaSecondOutlineSize = std::round(deltaSecondOutlineSize * 10) / 10;
    deltaOutlineColor.round();
    deltaSecondOutlineColor.round();

    // return false if the animation is over
    if (deltaCornerRadius == 0
        && deltaOutlineSize == 0
        && deltaSecondOutlineSize == 0
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
    outlineSize += deltaOutlineSize;
    secondOutlineSize += deltaSecondOutlineSize;
    outlineColor += deltaOutlineColor;
    secondOutlineColor += deltaSecondOutlineColor;

    // check boundaries after adjusting
    clamp(cornerRadius, deltaCornerRadius, configCornerRadius);
    clamp(outlineSize, deltaOutlineSize, configOutlineSize);
    clamp(secondOutlineSize, deltaSecondOutlineSize, configSecondOutlineSize);
    outlineColor.clamp();
    secondOutlineColor.clamp();

    // the animation is still in progress
#ifdef DEBUG_ANIMATION
    repaintCount++;
#endif
    w.addRepaintFull();
}

#ifdef QT_DEBUG
QDebug KWin::operator<<(QDebug& debug, const KWin::EffectWindow& w) {
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
