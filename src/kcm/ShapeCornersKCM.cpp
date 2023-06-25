#include <QDialog>
#include <QVBoxLayout>

#include <kwineffects.h>
#include "kwineffects_interface.h"
#include "ShapeCornersKCM.h"

#include <QDBusConnection>

ShapeCornersKCM::ShapeCornersKCM(QWidget* parent, const QVariantList& args)
    : KCModule(parent, args)
    , ui(new Ui::Form)
{
    ui->setupUi(this);
    addConfig(ShapeCornersConfig::self(), widget());
}

void
ShapeCornersKCM::save()
{
    ShapeCornersConfig::self()->save();
    KCModule::save();
    OrgKdeKwinEffectsInterface interface(QStringLiteral("org.kde.KWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("kwin4_effect_shapecorners"));
}

ShapeCornersKCM::~ShapeCornersKCM() {
    delete ui;
}
