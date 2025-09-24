#include "KCM.h"

// KF5
#include <KPluginFactory>

namespace ShapeCorners
{
#if (QT_VERSION_MAJOR >= 6)
    K_PLUGIN_CLASS_WITH_JSON(KCM, "metadata.json")
#else
    K_PLUGIN_FACTORY_WITH_JSON(ShapeCornersConfigFactory, "metadata.json", registerPlugin<KCM>();)
#endif
} // namespace ShapeCorners

#include "plugin.moc"
