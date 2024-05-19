#include "ShapeCornersEffect.h"

namespace ShapeCorners {
    Q_NAMESPACE
    
    KWIN_EFFECT_FACTORY_SUPPORTED(  ShapeCornersEffect,
                                    "metadata.json",
                                    return ShapeCornersEffect::supported();)
}

#include "plugin.moc"
