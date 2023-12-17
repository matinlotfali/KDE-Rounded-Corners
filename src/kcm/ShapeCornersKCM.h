#pragma once

#include <kcmodule.h>
#include "ui_ShapeCornersKCM.h"
#include "ShapeCornersConfig.h"

class ShapeCornersKCM final: public KCModule
{
    Q_OBJECT
public:
    explicit ShapeCornersKCM(QWidget* parent = nullptr, const QVariantList& args = QVariantList());
    ~ShapeCornersKCM() override;

public slots:
    void defaults() override;
    void load() override;
    void save() override;
    void update_colors();
    void update_windows();

private:
    Ui::Form *ui;
};
