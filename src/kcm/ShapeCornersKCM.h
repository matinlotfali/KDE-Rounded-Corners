#pragma once

#include <kcmodule.h>
#include "ShapeCornersConfig.h"

namespace Ui {
    class Form;
}

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
    void update_colors() const;
    void update_windows() const;

private:
    Ui::Form *ui;
};
