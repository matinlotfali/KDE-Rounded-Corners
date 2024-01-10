#include "ShapeCornersKCM.h"

// KF5
#include <KPluginFactory>

#if (QT_VERSION_MAJOR >= 248)
K_PLUGIN_CLASS(ShapeCornersKCM)
#else
K_PLUGIN_FACTORY_WITH_JSON(ShapeCornersConfigFactory,
                           "metadata.json",
                           registerPlugin<ShapeCornersKCM>();)
#endif

#include "plugin.moc"