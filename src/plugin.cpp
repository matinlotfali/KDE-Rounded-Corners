#include "ShapeCornersEffect.h"

KWIN_EFFECT_FACTORY_SUPPORTED_ENABLED(  ShapeCornersEffect,
                                        "metadata.json",
                                        return ShapeCornersEffect::supported();,
                                        return ShapeCornersEffect::enabledByDefault();)

#include "plugin.moc"
