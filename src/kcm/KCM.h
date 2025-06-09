#pragma once

#include <kcmodule.h>
#include <KLocalizedString>
#include "Config.h"

namespace Ui {
    class Form;
}
class QListWidgetItem;

namespace ShapeCorners {
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
        void outline_group_toggled(bool value) const;
        void include_button_clicked();
        void exclude_button_clicked();

    private:
        std::shared_ptr<Ui::Form> ui;
        Config config;

        void load_ui() const;
        void add_inclusion(const QString& text);
        void add_exclusion(const QString& text);
        [[nodiscard]] QListWidgetItem* find_class_in_lists(const QString& text) const;

#if (QT_VERSION_MAJOR >= 6)
        QWidget* widget() final override { return KCModule::widget(); }
        const QPalette& palette() { return widget()->palette(); };
#endif
    };
}
