#include "KCM.h"

// KF6
#include <KPluginFactory>

namespace ShapeCorners
{
    K_PLUGIN_CLASS_WITH_JSON(KCM, "metadata.json")
} // namespace ShapeCorners

#include "plugin.moc"
