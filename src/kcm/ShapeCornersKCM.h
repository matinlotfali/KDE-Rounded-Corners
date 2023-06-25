 
#include <kcmodule.h>
#include "ui_ShapeCornersKCM.h"
#include "ShapeCornersConfig.h"

class ShapeCornersKCM : public KCModule
{
    Q_OBJECT
public:
    explicit ShapeCornersKCM(QWidget* parent = nullptr, const QVariantList& args = QVariantList());
    ~ShapeCornersKCM() override;

public slots:
    void save() override;

private:
    Ui::Form *ui;
};
