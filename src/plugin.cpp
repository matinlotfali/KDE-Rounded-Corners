#include "Effect.h"

namespace ShapeCorners {
    Q_NAMESPACE
    
    KWIN_EFFECT_FACTORY_SUPPORTED(Effect,
                                  "metadata.json",
                                  return Effect::supported();)
}

#include "plugin.moc"
