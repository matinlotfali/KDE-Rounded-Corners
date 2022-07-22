#include "shapecorners.h"

namespace KWin
{
    Q_NAMESPACE

#if KWIN_EFFECT_API_VERSION < 233
KWIN_EFFECT_FACTORY_SUPPORTED_ENABLED(  ShapeCornersFactory,
                                        ShapeCornersEffect,
                                        "shapecorners.json",
                                        return ShapeCornersEffect::supported();,
                                        return ShapeCornersEffect::enabledByDefault();)
#else
KWIN_EFFECT_FACTORY_SUPPORTED_ENABLED(  ShapeCornersEffect,
                                        "shapecorners.json",
                                        return ShapeCornersEffect::supported();,
                                        return ShapeCornersEffect::enabledByDefault();)
#endif

}

#include "main.moc"
