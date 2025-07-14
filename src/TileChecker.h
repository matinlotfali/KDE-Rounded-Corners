/**
 * @file TileChecker.h
 * @brief Declares the ShapeCorners::TileChecker class for checking and marking tiled windows.
 *
 * This file provides the TileChecker class, which recursively checks window tiling arrangements
 * and marks windows as tiled for the ShapeCorners KWin effect.
 */

#pragma once

#include <WindowManager.h>
#include <cstdint>

class QRect;
namespace KWin {
    class EffectWindow;
}

namespace ShapeCorners {
    class Window;

    /**
     * @class TileChecker
     * @brief Checks and marks tiled windows in the ShapeCorners KWin effect.
     *
     * Recursively checks window arrangements to determine if windows are tiled,
     * and marks them accordingly. Supports both horizontal and vertical tiling checks.
     */
    class TileChecker {

    public:
        /**
         * @brief Constructs a TileChecker with a reference to the managed window list.
         * @param windowList Reference to the managed window list.
         */
        explicit TileChecker(WindowList& windowList): m_managed(windowList) {}

        /**
         * @brief Clears the tiled state for all managed windows.
         */
        void clearTiles() const noexcept;

        /**
         * @brief Checks and marks tiled windows based on the given screen geometry.
         * @param screen The QRect representing the screen area to check.
         */
        void checkTiles(const QRect& screen) noexcept;

    private:
        /**
         * @brief Reference to the managed window list.
         */
        WindowList& m_managed;

        /**
         * @brief End coordinate of the screen (x or y, depending on orientation).
         */
        double screen_end = -1;

        /**
         * @brief Gap between tiles.
         */
        double gap = -1;

        /**
         * @brief Recursively checks for a chain of tiled windows.
         * @tparam vertical If true, checks vertical tiling; otherwise, checks horizontal tiling.
         * @param window_start The starting coordinate for the check.
         * @param depth Current recursion depth.
         * @return True if a valid chain of tiles is found, false otherwise.
         */
        template<bool vertical>
        bool checkTiled_Recursive(double window_start, uint8_t depth) noexcept;
    };
}
