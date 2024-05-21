#pragma once

#include <kcmodule.h>
#include "Config.h"

namespace Ui {
    class Form;
}

namespace ShapeCorners {
    class Config;

    class KCM final : public KCModule {
    Q_OBJECT

    public:
#if (QT_VERSION_MAJOR >= 6)
        explicit KCM(QObject *parent, const KPluginMetaData &args);
#else
        explicit KCM(QWidget* parent = nullptr, const QVariantList& args = QVariantList());
#endif

    public Q_SLOTS:
        void defaults() override;
        void load() override;
        void save() override;
        void update_colors();
        void update_windows() const;

    private:
        std::shared_ptr<Ui::Form> ui;
        Config config;

#if (QT_VERSION_MAJOR >= 6)
        const QPalette &palette() { return widget()->palette(); };
#endif
    };
}
