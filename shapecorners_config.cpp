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
        , shadowColor("shadowColor")
        , outlineColor("outlineColor")
        , outlineThickness("outlineThickness")
        , defaultRoundness(5)
        , defaultShadows(false)
        , defaultShadowColor(QColor(Qt::black))
        , defaultOutlineColor(QColor(Qt::black))
        , defaultOutlineThickness(1.0f)
    {}
    ShapeCornersConfig *q;
    QString roundness, dsp, shadowColor,
            outlineColor, outlineThickness;
    QVariant defaultRoundness, defaultShadows, defaultShadowColor,
            defaultOutlineColor, defaultOutlineThickness;
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
    QColor shadowColor = conf.readEntry(d->shadowColor, d->defaultShadowColor).value<QColor>();
    d->ui->drawShadowEnabled->setChecked(shadowColor.alpha() > 0);
    shadowColor.setAlpha(255);
    d->ui->shadowColor->setColor(shadowColor);
    QColor outlineColor = conf.readEntry(d->outlineColor, d->defaultOutlineColor).value<QColor>();
    d->ui->drawOutlineEnabled->setChecked(outlineColor.alpha() > 0);
    d->ui->outlineColor->setColor(outlineColor);
    d->ui->outlineThickness->setValue(conf.readEntry(d->outlineThickness, d->defaultOutlineThickness).toFloat());
    emit changed(false);
}

void
ShapeCornersConfig::save()
{
    KCModule::save();
    KConfigGroup conf = KSharedConfig::openConfig("shapecorners.conf")->group("General");
    conf.writeEntry(d->roundness, d->ui->roundness->value());
    conf.writeEntry(d->dsp, d->ui->dsp->isChecked());
    auto shadowColor = d->ui->shadowColor->color();
    shadowColor.setAlpha(d->ui->drawShadowEnabled->isChecked()? 255: 0);
    conf.writeEntry(d->shadowColor, shadowColor);
    auto outlineColor = d->ui->outlineColor->color();
    if(!d->ui->drawOutlineEnabled->isChecked())
        outlineColor.setAlpha(0);
    conf.writeEntry(d->outlineColor, outlineColor);
    conf.writeEntry(d->outlineThickness, d->ui->outlineThickness->value());
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
    d->ui->shadowColor->setColor(d->defaultShadowColor.value<QColor>());
    d->ui->drawShadowEnabled->setChecked(d->defaultShadowColor.value<QColor>().alpha() > 0);
    d->ui->outlineColor->setColor(d->defaultOutlineColor.value<QColor>());
    d->ui->drawOutlineEnabled->setChecked(d->defaultOutlineColor.value<QColor>().alpha() > 0);
    d->ui->outlineThickness->setValue(d->defaultOutlineThickness.toFloat());
    emit changed(true);
}

#include "shapecorners_config.moc"
