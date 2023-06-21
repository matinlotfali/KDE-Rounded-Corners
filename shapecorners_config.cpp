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
K_PLUGIN_FACTORY_WITH_JSON(ShapeCornersConfigFactory,
                           "shapecorners_config.json",
                           registerPlugin<ShapeCornersConfig>();)
#endif

class ShapeCornersConfig::ConfigDialog : public QWidget , public Ui::Form
{
public:
    explicit ConfigDialog(QWidget *parent) : QWidget(parent)
{
    setupUi(this);
}
};

ShapeCornersConfig::ShapeCornersConfig(QWidget* parent, const QVariantList& args)
    : KCModule(parent, args)
    , ui(new ConfigDialog(this))
{
    auto* layout = new QVBoxLayout(this);
    layout->addWidget(ui);
    setLayout(layout);
}

ShapeCornersConfig::~ShapeCornersConfig() = default;

void
ShapeCornersConfig::load()
{
    KCModule::load();
    m_config.Load();
    ui->roundness->setValue(m_config.m_size);

    ui->activeShadowColor->setColor(m_config.m_shadowColor);
    ui->inactiveShadowColor->setColor(m_config.m_inactiveShadowColor);
    ui->activeShadowSize->setValue(m_config.m_shadowSize);
    ui->inactiveShadowSize->setValue(m_config.m_inactiveShadowSize);

    QColor outlineColor = m_config.m_outlineColor;
    QColor inactiveOutlineColor = m_config.m_inactiveOutlineColor;
    ui->drawOutlineEnabled->setChecked(outlineColor.alpha() > 0);
    ui->outlineColor->setColor(outlineColor);
    ui->inactiveOutlineColor->setColor(inactiveOutlineColor);
    ui->outlineThickness->setValue(m_config.m_outlineThickness);
    emit changed(false);
}

void
ShapeCornersConfig::save()
{
    KCModule::save();
    m_config.m_size = ui->roundness->value();

    m_config.m_shadowColor = ui->activeShadowColor->color();
    m_config.m_shadowSize = ui->activeShadowSize->value();
    m_config.m_inactiveShadowColor = ui->inactiveShadowColor->color();
    m_config.m_inactiveShadowSize = ui->inactiveShadowSize->value();

    m_config.m_outlineColor = ui->outlineColor->color();
    m_config.m_inactiveOutlineColor = ui->inactiveOutlineColor->color();
    if(!ui->drawOutlineEnabled->isChecked()){
        m_config.m_outlineColor.setAlpha(0);
        m_config.m_inactiveOutlineColor.setAlpha(0);
    }
    m_config.m_outlineThickness = ui->outlineThickness->value();
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
    m_config = ConfigModel();
    ui->roundness->setValue(m_config.m_size);

    ui->activeShadowColor->setColor(m_config.m_shadowColor);
    ui->inactiveShadowColor->setColor(m_config.m_inactiveShadowColor);
    ui->activeShadowSize->setValue(m_config.m_shadowSize);
    ui->inactiveShadowSize->setValue(m_config.m_inactiveShadowSize);

    QColor outlineColor = m_config.m_outlineColor;
    QColor inactiveOutlineColor = m_config.m_inactiveOutlineColor;
    ui->drawOutlineEnabled->setChecked(outlineColor.alpha() > 0);
    ui->outlineColor->setColor(outlineColor);
    ui->inactiveOutlineColor->setColor(inactiveOutlineColor);
    ui->outlineThickness->setValue(m_config.m_outlineThickness);
    emit changed(true);
}

#include "shapecorners_config.moc"
