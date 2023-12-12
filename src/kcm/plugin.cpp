#include "ShapeCornersKCM.h"

// KF5
#include <KPluginFactory>

K_PLUGIN_FACTORY_WITH_JSON(ShapeCornersConfigFactory,
                           "metadata.json",
                           registerPlugin<ShapeCornersKCM>();)

#include "plugin.moc"