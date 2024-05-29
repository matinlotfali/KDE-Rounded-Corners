#include "KCM.h"
#include "ui_KCM.h"
#include "kwineffects_interface.h"

#if (QT_VERSION_MAJOR >= 6)
ShapeCorners::KCM::KCM(QObject* parent, const KPluginMetaData& args)
    : KCModule(parent, args)
    , ui(new Ui::Form)
{
    ui->setupUi(widget());
    addConfig(&config, widget());
#else
ShapeCorners::KCM::KCM(QWidget* parent, const QVariantList& args)
    : KCModule(parent, args)
    , ui(new Ui::Form)
{
    ui->setupUi(this);
    addConfig(&config, this);
#endif

    update_colors();
    update_windows();

    QPixmap pix (16, 16);
    for (int index = 0; index < ui->kcfg_ActiveOutlinePalette->count(); ++index) {
        auto c = palette().color(QPalette::Active, static_cast<QPalette::ColorRole>(index));
        pix.fill(c);
        QIcon icon (pix);
        ui->kcfg_ActiveOutlinePalette->setItemIcon(index, icon);
        ui->kcfg_ActiveSecondOutlinePalette->setItemIcon(index, icon);

        c = palette().color(QPalette::Inactive, static_cast<QPalette::ColorRole>(index));
        pix.fill(c);
        QIcon icon2 (pix);
        ui->kcfg_InactiveOutlinePalette->setItemIcon(index, icon2);
        ui->kcfg_InactiveSecondOutlinePalette->setItemIcon(index, icon2);
    }

    connect(ui->kcfg_ActiveOutlinePalette, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &KCM::update_colors);
    connect(ui->kcfg_InactiveOutlinePalette, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &KCM::update_colors);
    connect(ui->kcfg_OutlineColor, &KColorButton::changed, this, &KCM::update_colors);
    connect(ui->kcfg_InactiveOutlineColor, &KColorButton::changed, this, &KCM::update_colors);
    connect(ui->kcfg_ActiveOutlineUsePalette, &QRadioButton::toggled, this, &KCM::update_colors);
    connect(ui->kcfg_InactiveOutlineUsePalette, &QRadioButton::toggled, this, &KCM::update_colors);

    connect(ui->kcfg_ActiveSecondOutlinePalette, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &KCM::update_colors);
    connect(ui->kcfg_InactiveSecondOutlinePalette, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &KCM::update_colors);
    connect(ui->kcfg_SecondOutlineColor, &KColorButton::changed, this, &KCM::update_colors);
    connect(ui->kcfg_InactiveSecondOutlineColor, &KColorButton::changed, this, &KCM::update_colors);
    connect(ui->kcfg_ActiveSecondOutlineUsePalette, &QRadioButton::toggled, this, &KCM::update_colors);
    connect(ui->kcfg_InactiveSecondOutlineUsePalette, &QRadioButton::toggled, this, &KCM::update_colors);

    // It was expected that the Apply button would get enabled automatically as the gradient sliders move, but it doesn't.
    // Maybe it is a bug on the KCM side. Need to check and delete these lines later.
    connect(ui->kcfg_ActiveShadowAlpha, &KGradientSelector::sliderMoved, this, &KCM::markAsChanged);
    connect(ui->kcfg_InactiveShadowAlpha, &KGradientSelector::sliderMoved, this, &KCM::markAsChanged);
    connect(ui->kcfg_ActiveOutlineAlpha, &KGradientSelector::sliderMoved, this, &KCM::markAsChanged);
    connect(ui->kcfg_InactiveOutlineAlpha, &KGradientSelector::sliderMoved, this, &KCM::markAsChanged);
    connect(ui->kcfg_ActiveSecondOutlineAlpha, &KGradientSelector::sliderMoved, this, &KCM::markAsChanged);
    connect(ui->kcfg_InactiveSecondOutlineAlpha, &KGradientSelector::sliderMoved, this, &KCM::markAsChanged);

    connect(ui->primaryOutline, &QGroupBox::toggled, this, &KCM::outline_group_toggled);
    connect(ui->secondaryOutline, &QGroupBox::toggled, this, &KCM::outline_group_toggled);

    connect(ui->refreshButton, &QPushButton::pressed, this, &KCM::update_windows);
    connect(ui->includeButton, &QPushButton::pressed, [=, this]() {
        if (const auto s = ui->currentWindowList->currentItem();
            s && ui->InclusionList->findItems(s->text(), Qt::MatchExactly).empty()) {
            ui->InclusionList->addItem(s->text());
            markAsChanged();
        }
    });
    connect(ui->excludeButton, &QPushButton::pressed, [=, this]() {
        if (const auto s = ui->currentWindowList->currentItem();
            s && ui->ExclusionList->findItems(s->text(), Qt::MatchExactly).empty()) {
            ui->ExclusionList->addItem(s->text());
            markAsChanged();
        }
    });
    connect(ui->deleteIncludeButton, &QPushButton::pressed, [=, this]() {
        ui->InclusionList->takeItem(ui->InclusionList->currentRow());
        markAsChanged();
    });
    connect(ui->deleteExcludeButton, &QPushButton::pressed, [=, this]() {
        ui->ExclusionList->takeItem(ui->ExclusionList->currentRow());
        markAsChanged();
    });
}

void
ShapeCorners::KCM::save()
{
    QStringList inclusions, exclusions;
    for (int i = 0; i < ui->InclusionList->count(); ++i)
        inclusions.push_back(ui->InclusionList->item(i)->text());
    config.setInclusions(inclusions);
    for (int i = 0; i < ui->ExclusionList->count(); ++i)
        exclusions.push_back(ui->ExclusionList->item(i)->text());
    config.setExclusions(exclusions);

    qDebug() << "ShapeCorners: Saving configurations";
    config.save();
    KCModule::save();

    const auto dbusName = QStringLiteral("kwin4_effect_shapecorners");
    OrgKdeKwinEffectsInterface interface(QStringLiteral("org.kde.KWin"),
                                         QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(dbusName);

    // It was expected that the Apply button would repaint the whole screen, but it doesn't.
    // Even calling KWin::effects->addRepaintFull(); didn't do the trick.
    // Maybe it is a bug on the KWin side. Need to check and delete these lines later.
    interface.unloadEffect(dbusName);
    interface.loadEffect(dbusName);

    load();
}

void ShapeCorners::KCM::update_colors() {
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

    checked = ui->kcfg_ActiveSecondOutlineUsePalette->isChecked();
    index = ui->kcfg_ActiveSecondOutlinePalette->currentIndex();
    color = checked ? palette().color(QPalette::Active, static_cast<QPalette::ColorRole>(index)): ui->kcfg_SecondOutlineColor->color();
    ui->kcfg_ActiveSecondOutlineAlpha->setSecondColor(color);

    checked = ui->kcfg_InactiveSecondOutlineUsePalette->isChecked();
    index = ui->kcfg_InactiveSecondOutlinePalette->currentIndex();
    color = checked ? palette().color(QPalette::Inactive, static_cast<QPalette::ColorRole>(index)): ui->kcfg_InactiveSecondOutlineColor->color();
    ui->kcfg_InactiveSecondOutlineAlpha->setSecondColor(color);
}

void ShapeCorners::KCM::update_windows() const {
    QStringList windowList;
    if (const auto connection = QDBusConnection::sessionBus(); connection.isConnected())
        if (QDBusInterface interface(QStringLiteral("org.kde.ShapeCorners"), QStringLiteral("/ShapeCornersEffect")); interface.isValid())
            if (const QDBusReply<QString> reply = interface.call(QStringLiteral("get_window_titles")); reply.isValid())
                windowList = reply.value().split(QStringLiteral("\n"));

    ui->currentWindowList->clear();
    ui->currentWindowList->addItems(windowList);
}

void ShapeCorners::KCM::outline_group_toggled(bool value) const {
    if (sender() == ui->primaryOutline) {
        ui->kcfg_OutlineThickness->setValue(value ? 0.75 : 0);
        ui->kcfg_InactiveOutlineThickness->setValue(value ? 0.75 : 0);
    } else if (sender() == ui->secondaryOutline) {
        ui->kcfg_SecondOutlineThickness->setValue(value ? 0.75 : 0);
        ui->kcfg_InactiveSecondOutlineThickness->setValue(value ? 0.75 : 0);
    }
}

void ShapeCorners::KCM::load() {
    KCModule::load();
    config.load();
    load_ui();
}

void ShapeCorners::KCM::defaults() {
    KCModule::defaults();
    config.setDefaults();
    load_ui();
}

void ShapeCorners::KCM::load_ui() const {
    ui->InclusionList->clear();
    ui->ExclusionList->clear();

    ui->InclusionList->addItems(config.inclusions());
    ui->ExclusionList->addItems(config.exclusions());

    ui->primaryOutline->setChecked(ui->kcfg_OutlineThickness->value() > 0);
    ui->secondaryOutline->setChecked(ui->kcfg_SecondOutlineThickness->value() > 0);
}
