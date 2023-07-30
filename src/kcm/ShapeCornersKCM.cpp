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
    addConfig(ShapeCornersConfig::self(), this);
    update_colors();

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
