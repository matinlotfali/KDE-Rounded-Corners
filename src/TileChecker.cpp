
#include "TileChecker.h"
#include "Window.h"
#include <ranges>
#if QT_VERSION_MAJOR >= 6
#include <effect/effecthandler.h>
#include <utility>
#else
#include <kwineffects.h>
#endif

/**
 * @brief Maximum recursion depth for tile checking.
 */
constexpr static uint8_t MAX_TILE_DEPTH = 5;

/**
 * @brief Maximum gap size allowed between tiles
 */
constexpr static uint8_t MAX_GAP_SIZE = 40;

template<bool vertical>
bool ShapeCorners::TileChecker::checkTiled_Recursive(double window_start, const uint8_t depth) noexcept {
    if (window_start == screen_end) {
        return true;    // Found the last chain of tiles
    }
    if (window_start > screen_end) {
        return false;
    }
    if (depth > MAX_TILE_DEPTH) {
        return false;
    }

    bool found_last_chain = false;
    for (auto& [kwindow, window]: m_managed) {

        // Skip windows without an effect
        if (!window->hasEffect()) {
            continue;
        }

        // Select x/y and width/height based on orientation
        const auto orientation_x = std::get<vertical>(std::make_pair(kwindow->x(), kwindow->y()));
        const auto orientation_width = std::get<vertical>(std::make_pair(kwindow->width(), kwindow->height()));

        if (depth == 0) {
            // Ignore windows with a gap larger than allowed
            if(orientation_x - window_start > MAX_GAP_SIZE) {
                continue;
            }
            gap = orientation_x - window_start;
            window_start += gap;
        }

        // If the window starts at the expected position and has a positive size
        if (orientation_x == window_start && orientation_width > 0) {
            // Recursively check the next tile in the chain
            if (checkTiled_Recursive<vertical>(window_start + orientation_width + gap, depth+1)) {
                window->isTiled = true;   // Mark every tile as you go back to the first.
                found_last_chain = true;
            }
        }

        // Revert changes to window_start for the next iteration
        if(depth == 0) {
            window_start -= gap;
        }
    }
    return found_last_chain;
}

void ShapeCorners::TileChecker::clearTiles() const noexcept {
    // Iterate over all managed windows and reset their isTiled flag
    for (const auto & window: m_managed | std::views::values) {
        window->isTiled = false;
    }
}

void ShapeCorners::TileChecker::checkTiles(const QRect& screen) noexcept {
    // Check horizontally
    screen_end = screen.x() + screen.width();
    checkTiled_Recursive<false>(screen.x(), 0);

    // Check vertically
    screen_end = screen.y() + screen.height();
    checkTiled_Recursive<true>(screen.y(), 0);
}
