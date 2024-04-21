#pragma once

#include <kcmodule.h>

namespace Ui {
    class Form;
}

class ShapeCornersKCM final: public KCModule
{
    Q_OBJECT
public:
#if (QT_VERSION_MAJOR >= 6)
    explicit ShapeCornersKCM(QObject* parent, const KPluginMetaData& args);
#else
    explicit ShapeCornersKCM(QWidget* parent = nullptr, const QVariantList& args = QVariantList());
#endif

public Q_SLOTS:
    void defaults() override;
    void load() override;
    void save() override;
    void update_colors();
    void update_windows() const;

private:
    std::shared_ptr<Ui::Form> ui;

#if (QT_VERSION_MAJOR >= 6)
    const QPalette& palette() { return widget()->palette(); };
#endif
};
