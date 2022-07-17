#include "shapecorners_config.h"
#include "ui_shapecorners_config.h"

#include <QDialog>
#include <QVBoxLayout>

#include <kwineffects.h>
#include <kwineffects_interface.h>

#include <QDBusConnection>
#include <QDBusMessage>
#include <QDBusArgument>

#include <KConfigGroup>
#include <KPluginFactory>
#include <KAboutData>

#include "kcoreaddons_version.h"
#if KCOREADDONS_VERSION >= QT_VERSION_CHECK(5, 90, 0)
K_PLUGIN_CLASS(ShapeCornersConfig)
#else
K_PLUGIN_FACTORY(ShapeCornersConfigFactory, registerPlugin<ShapeCornersConfig>();)
#endif

class ConfigDialog : public QWidget , public Ui::Form
{
public:
    explicit ConfigDialog(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
}
};

class ShapeCornersConfig::Private
{
public:
    Private(ShapeCornersConfig *config)
        : q(config)
        , roundness("roundness")
        , dsp("dsp")
        , drawShadow("drawShadow")
        , defaultRoundness(5)
        , defaultShadows(false)
        , defaultDrawShadow(true)
    {}
    ShapeCornersConfig *q;
    QString roundness, dsp, drawShadow;
    QVariant defaultRoundness, defaultShadows, defaultDrawShadow;
    ConfigDialog *ui;
};

ShapeCornersConfig::ShapeCornersConfig(QWidget* parent, const QVariantList& args)
    : KCModule(parent, args)
    , d(new Private(this))
{
    setAboutData(new KAboutData("ShapeCorners","ShapeCorners", "git"));
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(d->ui = new ConfigDialog(this));
    setLayout(layout);
}

ShapeCornersConfig::~ShapeCornersConfig()
{
    delete d;
}

void
ShapeCornersConfig::load()
{
    KCModule::load();
    KConfigGroup conf = KSharedConfig::openConfig("shapecorners.conf")->group("General");
    d->ui->roundness->setValue(conf.readEntry(d->roundness, d->defaultRoundness).toInt());
    d->ui->dsp->setChecked(conf.readEntry(d->dsp, d->defaultShadows).toBool());
    d->ui->drawShadow->setChecked(conf.readEntry(d->drawShadow, d->defaultDrawShadow).toBool());
    emit changed(false);
}

void
ShapeCornersConfig::save()
{
    KCModule::save();
    KConfigGroup conf = KSharedConfig::openConfig("shapecorners.conf")->group("General");
    conf.writeEntry(d->roundness, d->ui->roundness->value());
    conf.writeEntry(d->dsp, d->ui->dsp->isChecked());
    conf.writeEntry(d->drawShadow, d->ui->drawShadow->isChecked());
    conf.sync();
    emit changed(false);
    OrgKdeKwinEffectsInterface interface(QStringLiteral("org.kde.KWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("kwin4_effect_shapecorners"));
}

void
ShapeCornersConfig::defaults()
{
    KCModule::defaults();
    d->ui->roundness->setValue(d->defaultRoundness.toInt());
    d->ui->dsp->setChecked(d->defaultShadows.toBool());
    d->ui->drawShadow->setChecked(d->defaultDrawShadow.toBool());
    emit changed(true);
}

#include "shapecorners_config.moc"
