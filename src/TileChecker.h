//
// Created by matin on 16/06/24.
//

#ifndef KWIN4_EFFECT_SHAPECORNERS_TILECHECKER_H
#define KWIN4_EFFECT_SHAPECORNERS_TILECHECKER_H

#include <unordered_map>
#include <cstdint>

class QRect;
namespace KWin {
    class EffectWindow;
}

namespace ShapeCorners {
    class Window;
    using WindowList = std::unordered_map<const KWin::EffectWindow *, Window>;

    class TileChecker {

    public:
        explicit TileChecker(WindowList& windowList): m_managed(windowList) {};

        void clearTiles();
        void checkTiles(const QRect& screen);

    private:
        constexpr static uint8_t max_tile_depth = 5;
        WindowList& m_managed;

        double screen_end = -1;
        double gap = -1;

        template<bool vertical>
        bool checkTiled_Recursive(double window_start, uint8_t depth);
    };
}


#endif //KWIN4_EFFECT_SHAPECORNERS_TILECHECKER_H
