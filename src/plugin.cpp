/**
 * @file plugin.cpp
 * @brief Plugin entry point for the ShapeCorners KWin effect.
 *
 * This file registers the ShapeCorners::Effect with KWin using the KWIN_EFFECT_FACTORY_SUPPORTED macro.
 * It provides the necessary factory and metadata for the effect to be loaded by KWin.
 */

#include "Effect.h"

namespace ShapeCorners {
    Q_NAMESPACE

    /**
     * @brief Registers the Effect class as a KWin effect plugin.
     *
     * Uses the KWIN_EFFECT_FACTORY_SUPPORTED macro to register the Effect class,
     * specifying the metadata file and the support check.
     */
    KWIN_EFFECT_FACTORY_SUPPORTED(Effect,
                                  "metadata.json",
                                  return Effect::supported();)
}

#include "plugin.moc"
