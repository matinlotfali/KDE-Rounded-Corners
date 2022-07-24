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
    {}
    ShapeCornersConfig *q;
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
    m_config.Load();
    d->ui->roundness->setValue(m_config.m_size);
    d->ui->dsp->setChecked(m_config.m_dsp);
    QColor shadowColor = m_config.m_shadowColor;
    d->ui->drawShadowEnabled->setChecked(shadowColor.alpha() > 0);
    shadowColor.setAlpha(255);
    d->ui->shadowColor->setColor(shadowColor);
    QColor outlineColor = m_config.m_outlineColor;
    d->ui->drawOutlineEnabled->setChecked(outlineColor.alpha() > 0);
    d->ui->outlineColor->setColor(outlineColor);
    d->ui->outlineThickness->setValue(m_config.m_outlineThickness);
    emit changed(false);
}

void
ShapeCornersConfig::save()
{
    KCModule::save();
    m_config.m_size = d->ui->roundness->value();
    m_config.m_dsp = d->ui->dsp->isChecked();
    m_config.m_shadowColor = d->ui->shadowColor->color();
    m_config.m_shadowColor.setAlpha(d->ui->drawShadowEnabled->isChecked()? 255: 0);
    m_config.m_outlineColor = d->ui->outlineColor->color();
    if(!d->ui->drawOutlineEnabled->isChecked())
        m_config.m_outlineColor.setAlpha(0);
    m_config.m_outlineThickness = d->ui->outlineThickness->value();
    m_config.Save();

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
    ConfigModel defaultConfig;
    d->ui->roundness->setValue(m_config.m_size);
    d->ui->dsp->setChecked(m_config.m_dsp);
    QColor shadowColor = m_config.m_shadowColor;
    d->ui->drawShadowEnabled->setChecked(shadowColor.alpha() > 0);
    shadowColor.setAlpha(255);
    d->ui->shadowColor->setColor(shadowColor);
    QColor outlineColor = m_config.m_outlineColor;
    d->ui->drawOutlineEnabled->setChecked(outlineColor.alpha() > 0);
    d->ui->outlineColor->setColor(outlineColor);
    d->ui->outlineThickness->setValue(m_config.m_outlineThickness);
    emit changed(true);
}

#include "shapecorners_config.moc"
