#include "KCM.h"
#include "kwineffects_interface.h"
#include "ui_KCM.h"

#if (QT_VERSION_MAJOR >= 6)
ShapeCorners::KCM::KCM(QObject *parent, const KPluginMetaData &args) : KCModule(parent, args), ui(new Ui::Form)
{
    // Set up the UI for Qt 6
    ui->setupUi(KCModule::widget());
    ui->widget->setFixedSize(800, 672);
    ui->currentWindowList->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    addConfig(&config, KCModule::widget());

#else
ShapeCorners::KCM::KCM(QWidget *parent, const QVariantList &args) : KCModule(parent, args), ui(new Ui::Form)
{
    // Set up the UI for Qt 5
    ui->setupUi(this);
    addConfig(&config, this);
#endif

    // Initialize color previews and window list
    update_colors();
    update_windows();

    // Set icons for palette combo boxes based on current palette colors
    constexpr int size = 16;
    QPixmap       pix(size, size);
    for (int index = 0; index < ui->kcfg_ActiveOutlinePalette->count(); ++index) {
        auto color = palette().color(QPalette::Active, static_cast<QPalette::ColorRole>(index));
        pix.fill(color);
        QIcon icon(pix);
        ui->kcfg_ActiveOutlinePalette->setItemIcon(index, icon);
        ui->kcfg_ActiveSecondOutlinePalette->setItemIcon(index, icon);
        ui->kcfg_ActiveShadowPalette->setItemIcon(index, icon);

        color = palette().color(QPalette::Inactive, static_cast<QPalette::ColorRole>(index));
        pix.fill(color);
        QIcon icon2(pix);
        ui->kcfg_InactiveOutlinePalette->setItemIcon(index, icon2);
        ui->kcfg_InactiveSecondOutlinePalette->setItemIcon(index, icon2);
        ui->kcfg_InactiveShadowPalette->setItemIcon(index, icon2);
    }

    // Connect UI signals to slots for color and palette changes
    connect(ui->kcfg_ActiveOutlinePalette, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &KCM::update_colors);
    connect(ui->kcfg_InactiveOutlinePalette, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &KCM::update_colors);
    connect(ui->kcfg_OutlineColor, &KColorButton::changed, this, &KCM::update_colors);
    connect(ui->kcfg_InactiveOutlineColor, &KColorButton::changed, this, &KCM::update_colors);
    connect(ui->kcfg_ActiveOutlineUsePalette, &QRadioButton::toggled, this, &KCM::update_colors);
    connect(ui->kcfg_InactiveOutlineUsePalette, &QRadioButton::toggled, this, &KCM::update_colors);

    connect(ui->kcfg_ActiveSecondOutlinePalette, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &KCM::update_colors);
    connect(ui->kcfg_InactiveSecondOutlinePalette, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &KCM::update_colors);
    connect(ui->kcfg_SecondOutlineColor, &KColorButton::changed, this, &KCM::update_colors);
    connect(ui->kcfg_InactiveSecondOutlineColor, &KColorButton::changed, this, &KCM::update_colors);
    connect(ui->kcfg_ActiveSecondOutlineUsePalette, &QRadioButton::toggled, this, &KCM::update_colors);
    connect(ui->kcfg_InactiveSecondOutlineUsePalette, &QRadioButton::toggled, this, &KCM::update_colors);

    connect(ui->kcfg_ActiveShadowPalette, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &KCM::update_colors);
    connect(ui->kcfg_ActiveShadowPalette, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
            &KCM::update_colors);
    connect(ui->kcfg_ShadowColor, &KColorButton::changed, this, &KCM::update_colors);
    connect(ui->kcfg_InactiveShadowColor, &KColorButton::changed, this, &KCM::update_colors);
    connect(ui->kcfg_ActiveShadowUsePalette, &QRadioButton::toggled, this, &KCM::update_colors);
    connect(ui->kcfg_InactiveShadowUsePalette, &QRadioButton::toggled, this, &KCM::update_colors);

    // Enable/disable shadow controls based on native decoration shadow usage
    connect(ui->kcfg_UseNativeDecorationShadows, &QRadioButton::toggled, [=, this] {
        const auto &useCustomShadows = !ui->kcfg_UseNativeDecorationShadows->isChecked();
        ui->UseCustomShadows->setChecked(useCustomShadows);
        ui->activeShadowGroupBox->setEnabled(useCustomShadows);
        ui->inactiveShadowGroupBox->setEnabled(useCustomShadows);
    });

    // It was expected that the Apply button would get enabled automatically as the gradient sliders move, but it
    // doesn't. Maybe it is a bug on the KCM side. Need to check and delete these lines later.
    connect(ui->kcfg_ActiveShadowAlpha, &KGradientSelector::sliderMoved, this, &KCM::markAsChanged);
    connect(ui->kcfg_InactiveShadowAlpha, &KGradientSelector::sliderMoved, this, &KCM::markAsChanged);
    connect(ui->kcfg_ActiveOutlineAlpha, &KGradientSelector::sliderMoved, this, &KCM::markAsChanged);
    connect(ui->kcfg_InactiveOutlineAlpha, &KGradientSelector::sliderMoved, this, &KCM::markAsChanged);
    connect(ui->kcfg_ActiveSecondOutlineAlpha, &KGradientSelector::sliderMoved, this, &KCM::markAsChanged);
    connect(ui->kcfg_InactiveSecondOutlineAlpha, &KGradientSelector::sliderMoved, this, &KCM::markAsChanged);

    // Connect outline group toggles
    connect(ui->primaryOutline, &QGroupBox::toggled, this, &KCM::outline_group_toggled);
    connect(ui->secondaryOutline, &QGroupBox::toggled, this, &KCM::outline_group_toggled);

    // Connect window list and inclusion/exclusion controls
    connect(ui->refreshButton, &QPushButton::pressed, this, &KCM::update_windows);
    connect(ui->includeButton, &QPushButton::pressed, [=, this]() {
        // Add selected window to inclusion list if not already present
        if (const auto *const item = ui->currentWindowList->currentItem();
            item != nullptr && !item->text().isEmpty() &&
            ui->InclusionList->findItems(item->text(), Qt::MatchExactly).empty()) {
            ui->InclusionList->addItem(item->text());
            markAsChanged();
        }
    });
    connect(ui->excludeButton, &QPushButton::pressed, [=, this]() {
        // Add selected window to exclusion list if not already present
        if (const auto *const item = ui->currentWindowList->currentItem();
            item != nullptr && !item->text().isEmpty() &&
            ui->ExclusionList->findItems(item->text(), Qt::MatchExactly).empty()) {
            ui->ExclusionList->addItem(item->text());
            markAsChanged();
        }
    });
    connect(ui->deleteIncludeButton, &QPushButton::pressed, [=, this]() {
        // Remove selected item from inclusion list
        ui->InclusionList->takeItem(ui->InclusionList->currentRow());
        markAsChanged();
    });
    connect(ui->deleteExcludeButton, &QPushButton::pressed, [=, this]() {
        // Remove selected item from exclusion list
        ui->ExclusionList->takeItem(ui->ExclusionList->currentRow());
        markAsChanged();
    });

    // Enable/disable animation
    connect(ui->AnimationCheckbox, &QRadioButton::toggled, [=, this] {
        ui->kcfg_AnimationDuration->setEnabled(ui->AnimationCheckbox->isChecked());
        markAsChanged();
    });
}

void ShapeCorners::KCM::save()
{
    // Gather inclusion list from UI and save to config
    QStringList inclusions;
    for (int i = 0; i < ui->InclusionList->count(); ++i) {
        inclusions.push_back(ui->InclusionList->item(i)->text());
    }
    config.setInclusions(inclusions);

    // Gather exclusion list from UI and save to config
    QStringList exclusions;
    for (int i = 0; i < ui->ExclusionList->count(); ++i) {
        exclusions.push_back(ui->ExclusionList->item(i)->text());
    }
    config.setExclusions(exclusions);

    // Set animation duration based on its checkbox
    if (!ui->kcfg_AnimationDuration->isEnabled()) {
        config.setAnimationDuration(0);
    }

    qDebug() << "ShapeCorners: Saving configurations";
    config.save();
    KCModule::save();

    // Notify KWin to reload the effect configuration via DBus
    const auto                 dbusName = QStringLiteral("kwin4_effect_shapecorners");
    OrgKdeKwinEffectsInterface interface(QStringLiteral("org.kde.KWin"), QStringLiteral("/Effects"),
                                         QDBusConnection::sessionBus());
    interface.reconfigureEffect(dbusName);

    // It was expected that the Apply button would repaint the whole screen, but it doesn't.
    // Even calling KWin::effects->addRepaintFull(); didn't do the trick.
    // Maybe it is a bug on the KWin side. Need to check and delete these lines later.
    interface.unloadEffect(dbusName);
    interface.loadEffect(dbusName);

    // Reload UI to reflect saved settings
    load();
}

void ShapeCorners::KCM::update_colors()
{
    QColor color;
    bool   checked;
    int    index;

    // Update active outline color preview
    checked = ui->kcfg_ActiveOutlineUsePalette->isChecked();
    index   = ui->kcfg_ActiveOutlinePalette->currentIndex();
    color   = checked ? palette().color(QPalette::Active, static_cast<QPalette::ColorRole>(index))
                      : ui->kcfg_OutlineColor->color();
    ui->kcfg_ActiveOutlineAlpha->setSecondColor(color);

    // Update inactive outline color preview
    checked = ui->kcfg_InactiveOutlineUsePalette->isChecked();
    index   = ui->kcfg_InactiveOutlinePalette->currentIndex();
    color   = checked ? palette().color(QPalette::Inactive, static_cast<QPalette::ColorRole>(index))
                      : ui->kcfg_InactiveOutlineColor->color();
    ui->kcfg_InactiveOutlineAlpha->setSecondColor(color);

    // Update active second outline color preview
    checked = ui->kcfg_ActiveSecondOutlineUsePalette->isChecked();
    index   = ui->kcfg_ActiveSecondOutlinePalette->currentIndex();
    color   = checked ? palette().color(QPalette::Active, static_cast<QPalette::ColorRole>(index))
                      : ui->kcfg_SecondOutlineColor->color();
    ui->kcfg_ActiveSecondOutlineAlpha->setSecondColor(color);

    // Update inactive second outline color preview
    checked = ui->kcfg_InactiveSecondOutlineUsePalette->isChecked();
    index   = ui->kcfg_InactiveSecondOutlinePalette->currentIndex();
    color   = checked ? palette().color(QPalette::Inactive, static_cast<QPalette::ColorRole>(index))
                      : ui->kcfg_InactiveSecondOutlineColor->color();
    ui->kcfg_InactiveSecondOutlineAlpha->setSecondColor(color);

    // Update active shadow color preview
    checked = ui->kcfg_ActiveShadowUsePalette->isChecked();
    index   = ui->kcfg_ActiveShadowPalette->currentIndex();
    color   = checked ? palette().color(QPalette::Active, static_cast<QPalette::ColorRole>(index))
                      : ui->kcfg_ShadowColor->color();
    ui->kcfg_ActiveShadowAlpha->setSecondColor(color);

    // Update inactive shadow color preview
    checked = ui->kcfg_InactiveShadowUsePalette->isChecked();
    index   = ui->kcfg_InactiveShadowPalette->currentIndex();
    color   = checked ? palette().color(QPalette::Inactive, static_cast<QPalette::ColorRole>(index))
                      : ui->kcfg_InactiveShadowColor->color();
    ui->kcfg_InactiveShadowAlpha->setSecondColor(color);
}

void ShapeCorners::KCM::update_windows() const
{
    QJsonArray array;
    // Query the ShapeCorners effect for the list of open windows via DBus
    if (const auto connection = QDBusConnection::sessionBus(); connection.isConnected()) {
        if (QDBusInterface interface(QStringLiteral("org.kde.ShapeCorners"), QStringLiteral("/ShapeCornersEffect"));
            interface.isValid()) {
            if (const QDBusReply<QString> reply = interface.call(QStringLiteral("get_window_titles"));
                reply.isValid()) {
                const auto str = reply.value();
                const auto doc = QJsonDocument::fromJson(str.toUtf8());
                array          = doc.array();
            }
        }
    }

    // Clear and repopulate the window list table
    ui->currentWindowList->clear();
    ui->currentWindowList->setRowCount(static_cast<int>(array.size()));
    ui->currentWindowList->setColumnCount(3);
    ui->currentWindowList->setHorizontalHeaderLabels(
            {QStringLiteral("Class 1"), QStringLiteral("Class 2"), QStringLiteral("Caption")});

    // Fill the table with window class and caption data
    for (int i = 0; i < array.size(); ++i) {
        const auto obj         = array.at(i).toObject();
        const auto windowClass = obj.value(QStringLiteral("class")).toString();
        const auto caption     = obj.value(QStringLiteral("caption")).toString();

        const auto  windowClassSplit = windowClass.split(QChar(QChar::Space));
        const auto &class1           = windowClassSplit.at(0);
        const auto &class2           = windowClassSplit.at(1);

        ui->currentWindowList->setItem(i, 0, new QTableWidgetItem(class1));
        ui->currentWindowList->setItem(i, 1, new QTableWidgetItem(class2));
        ui->currentWindowList->setItem(i, 2, new QTableWidgetItem(caption));
    }
}

void ShapeCorners::KCM::outline_group_toggled(const bool value) const
{
    constexpr float outlineThicknessMin = 0.75F;
    // Enable or disable outline thickness based on group toggle
    if (sender() == ui->primaryOutline) {
        ui->kcfg_OutlineThickness->setValue(value ? outlineThicknessMin : 0);
        ui->kcfg_InactiveOutlineThickness->setValue(value ? outlineThicknessMin : 0);
    } else if (sender() == ui->secondaryOutline) {
        ui->kcfg_SecondOutlineThickness->setValue(value ? outlineThicknessMin : 0);
        ui->kcfg_InactiveSecondOutlineThickness->setValue(value ? outlineThicknessMin : 0);
    }
}

void ShapeCorners::KCM::load()
{
    // Load settings from config and update UI
    KCModule::load();
    config.load();
    load_ui();
}

void ShapeCorners::KCM::defaults()
{
    // Reset settings to defaults and update UI
    KCModule::defaults();
    config.setDefaults();
    load_ui();
    markAsChanged();
}

void ShapeCorners::KCM::load_ui() const
{
    // Clear and repopulate inclusion/exclusion lists from config
    ui->InclusionList->clear();
    ui->ExclusionList->clear();

    ui->InclusionList->addItems(config.inclusions());
    ui->ExclusionList->addItems(config.exclusions());

    // Set outline group checkboxes based on thickness values
    ui->primaryOutline->setChecked(ui->kcfg_OutlineThickness->value() > 0);
    ui->secondaryOutline->setChecked(ui->kcfg_SecondOutlineThickness->value() > 0);

    // Set animation checkboxes based on its duration
    ui->AnimationCheckbox->setChecked(config.animationDuration() > 0);
}
