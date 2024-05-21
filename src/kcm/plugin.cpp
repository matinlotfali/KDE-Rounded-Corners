#include "KCM.h"

// KF5
#include <KPluginFactory>

namespace ShapeCorners {
#if (QT_VERSION_MAJOR >= 248)
    K_PLUGIN_CLASS(KCM)
#else
    K_PLUGIN_FACTORY_WITH_JSON(ShapeCornersConfigFactory,
                               "metadata.json",
                               registerPlugin<KCM>();)
#endif
}

#include "plugin.moc"