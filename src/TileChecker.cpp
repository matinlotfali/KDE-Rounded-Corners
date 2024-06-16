//
// Created by matin on 16/06/24.
//

#include "TileChecker.h"
#include "Window.h"

#if QT_VERSION_MAJOR >= 6
#include <effect/effecthandler.h>
#include <utility>
#else
#include <kwineffects.h>
#endif

template<bool vertical>
bool ShapeCorners::TileChecker::checkTiled_Recursive(double window_start, const uint8_t depth) {
    if (window_start == screen_end)
        return true;    // Found the last chain of tiles
    if (window_start > screen_end)
        return false;
    if (depth > max_tile_depth)
        return false;

    bool r = false;
    for (auto& [w, window]: m_managed) {

        if (!window.hasEffect())
            continue;

        const auto x = std::get<vertical>(std::make_pair(w->x(), w->y()));
        const auto width = std::get<vertical>(std::make_pair(w->width(), w->height()));

        if (depth == 0) {
            if(x - window_start > 40)        // There is no way that a window is tiled and has such a big gap.
                continue;
            gap = x - window_start;
            window_start += gap;
        }

        if (x == window_start && width > 0) {
            if (checkTiled_Recursive<vertical>(window_start + width + gap, depth+1)) {
                window.isTiled = true;   // Mark every tile as you go back to the first.
                r = true;
            }
        }

        if(depth == 0) {
            window_start -= gap;    // Revert changes.
        }
    }
    return r;
}

void ShapeCorners::TileChecker::clearTiles() {
    for (auto& [ptr, window]: m_managed) {     // Delete tile memory.
        window.isTiled = false;
    }
}

void ShapeCorners::TileChecker::checkTiles(const QRect& screen) {
    screen_end = screen.x() + screen.width();
    checkTiled_Recursive<false>(screen.x(), 0); // Check horizontally

    screen_end = screen.y() + screen.height();
    checkTiled_Recursive<true>(screen.y(), 0); // Check vertically
}
