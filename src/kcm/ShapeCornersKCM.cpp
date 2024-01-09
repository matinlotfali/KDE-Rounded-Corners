#include "ShapeCornersKCM.h"
#include "ui_ShapeCornersKCM.h"
#include "kwineffects_interface.h"
#include "ShapeCornersConfig.h"
#include <QDialog>
#include <QList>

#if (QT_VERSION_MAJOR >= 6)
ShapeCornersKCM::ShapeCornersKCM(QObject* parent, const KPluginMetaData& args)
    : KCModule(parent, args)
    , ui(new Ui::Form)
{
    ui->setupUi(widget());
    addConfig(ShapeCornersConfig::self(), widget());
#else
ShapeCornersKCM::ShapeCornersKCM(QWidget* parent, const QVariantList& args)
    : KCModule(parent, args)
    , ui(new Ui::Form)
{
    ui->setupUi(this);
    addConfig(ShapeCornersConfig::self(), this);
#endif

    update_colors();
    update_windows();

    QPixmap pix (16, 16);
    for (int index = 0; index < ui->kcfg_ActiveOutlinePalette->count(); ++index) {
        auto c = palette().color(QPalette::Active, static_cast<QPalette::ColorRole>(index));
        pix.fill(c);
        QIcon icon (pix);
        ui->kcfg_ActiveOutlinePalette->setItemIcon(index, icon);
        ui->kcfg_ActiveShadowPalette->setItemIcon(index, icon);

        c = palette().color(QPalette::Inactive, static_cast<QPalette::ColorRole>(index));
        pix.fill(c);
        QIcon icon2 (pix);
        ui->kcfg_InactiveOutlinePalette->setItemIcon(index, icon2);
        ui->kcfg_InactiveShadowPalette->setItemIcon(index, icon2);
    }

    connect(ui->kcfg_ActiveOutlinePalette, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ShapeCornersKCM::update_colors);
    connect(ui->kcfg_InactiveOutlinePalette, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ShapeCornersKCM::update_colors);
    connect(ui->kcfg_OutlineColor, &KColorButton::changed, this, &ShapeCornersKCM::update_colors);
    connect(ui->kcfg_InactiveOutlineColor, &KColorButton::changed, this, &ShapeCornersKCM::update_colors);
    connect(ui->kcfg_ActiveOutlineUsePalette, &QRadioButton::toggled, this, &ShapeCornersKCM::update_colors);
    connect(ui->kcfg_InactiveOutlineUsePalette, &QRadioButton::toggled, this, &ShapeCornersKCM::update_colors);

    connect(ui->kcfg_ActiveShadowPalette, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ShapeCornersKCM::update_colors);
    connect(ui->kcfg_ActiveShadowPalette, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ShapeCornersKCM::update_colors);
    connect(ui->kcfg_ShadowColor, &KColorButton::changed, this, &ShapeCornersKCM::update_colors);
    connect(ui->kcfg_InactiveShadowColor, &KColorButton::changed, this, &ShapeCornersKCM::update_colors);
    connect(ui->kcfg_ActiveShadowUsePalette, &QRadioButton::toggled, this, &ShapeCornersKCM::update_colors);
    connect(ui->kcfg_InactiveShadowUsePalette, &QRadioButton::toggled, this, &ShapeCornersKCM::update_colors);

    connect(ui->refreshButton, &QPushButton::pressed, this, &ShapeCornersKCM::update_windows);
    connect(ui->includeButton, &QPushButton::pressed, [=, this]() {
        if (const auto s = ui->currentWindowList->currentItem();
            s && ui->InclusionList->findItems(s->text(), Qt::MatchExactly).empty())
            ui->InclusionList->addItem(s->text());
    });
    connect(ui->excludeButton, &QPushButton::pressed, [=, this]() {
        if (const auto s = ui->currentWindowList->currentItem();
            s && ui->ExclusionList->findItems(s->text(), Qt::MatchExactly).empty())
            ui->ExclusionList->addItem(s->text());
    });
    connect(ui->deleteIncludeButton, &QPushButton::pressed, [=, this]() {
        ui->InclusionList->takeItem(ui->InclusionList->currentRow());
    });
    connect(ui->deleteExcludeButton, &QPushButton::pressed, [=, this]() {
        ui->ExclusionList->takeItem(ui->ExclusionList->currentRow());
    });
}

void
ShapeCornersKCM::save()
{
    QStringList inclusions, exclusions;
    for (int i = 0; i < ui->InclusionList->count(); ++i)
        inclusions.push_back(ui->InclusionList->item(i)->text());
    for (int i = 0; i < ui->ExclusionList->count(); ++i)
        exclusions.push_back(ui->ExclusionList->item(i)->text());

    ShapeCornersConfig::setInclusions(inclusions);
    ShapeCornersConfig::setExclusions(exclusions);
    ShapeCornersConfig::self()->save();
    KCModule::save();
    OrgKdeKwinEffectsInterface interface(QStringLiteral("org.kde.KWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(QStringLiteral("kwin4_effect_shapecorners"));
}

void ShapeCornersKCM::update_colors() {
    QColor color;
    bool checked;
    int index;

    checked = ui->kcfg_ActiveOutlineUsePalette->isChecked();
    index = ui->kcfg_ActiveOutlinePalette->currentIndex();
    color = checked ? palette().color(QPalette::Active, static_cast<QPalette::ColorRole>(index)): ui->kcfg_OutlineColor->color();
    ui->kcfg_ActiveOutlineAlpha->setSecondColor(color);

    checked = ui->kcfg_InactiveOutlineUsePalette->isChecked();
    index = ui->kcfg_InactiveOutlinePalette->currentIndex();
    color = checked ? palette().color(QPalette::Inactive, static_cast<QPalette::ColorRole>(index)): ui->kcfg_InactiveOutlineColor->color();
    ui->kcfg_InactiveOutlineAlpha->setSecondColor(color);

    checked = ui->kcfg_ActiveShadowUsePalette->isChecked();
    index = ui->kcfg_ActiveShadowPalette->currentIndex();
    color = checked ? palette().color(QPalette::Active, static_cast<QPalette::ColorRole>(index)): ui->kcfg_ShadowColor->color();
    ui->kcfg_ActiveShadowAlpha->setSecondColor(color);

    checked = ui->kcfg_InactiveShadowUsePalette->isChecked();
    index = ui->kcfg_InactiveShadowPalette->currentIndex();
    color = checked ? palette().color(QPalette::Inactive, static_cast<QPalette::ColorRole>(index)): ui->kcfg_InactiveShadowColor->color();
    ui->kcfg_InactiveShadowAlpha->setSecondColor(color);
}

void ShapeCornersKCM::update_windows() const {
    QStringList windowList;
    if (const auto connection = QDBusConnection::sessionBus(); connection.isConnected())
        if (QDBusInterface interface(QStringLiteral("org.kde.ShapeCorners"), QStringLiteral("/ShapeCornersEffect")); interface.isValid())
            if (const QDBusReply<QString> reply = interface.call(QStringLiteral("get_window_titles")); reply.isValid())
                windowList = reply.value().split(QStringLiteral("\n"));

    ui->currentWindowList->clear();
    ui->currentWindowList->addItems(windowList);
}

void ShapeCornersKCM::load() {
    KCModule::load();
    ShapeCornersConfig::self()->load();
    ui->InclusionList->addItems(ShapeCornersConfig::inclusions());
    ui->ExclusionList->addItems(ShapeCornersConfig::exclusions());
}

void ShapeCornersKCM::defaults() {
    KCModule::defaults();
    ShapeCornersConfig::self()->setDefaults();
    ui->InclusionList->clear();
    ui->InclusionList->addItems(ShapeCornersConfig::inclusions());
    ui->ExclusionList->clear();
    ui->ExclusionList->addItems(ShapeCornersConfig::exclusions());
}
