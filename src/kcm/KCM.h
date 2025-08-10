#pragma once

#include <kcmodule.h>
#include <KLocalizedString>
#include "Config.h"

namespace Ui {
    class Form;
}

namespace ShapeCorners {

    /**
     * @class KCM
     * @brief KDE Control Module for configuring the ShapeCorners effect.
     *
     * This class provides the user interface and logic for configuring
     * the rounded corners effect in KDE via a KCM (KDE Control Module).
     * It manages loading, saving, and resetting settings, as well as
     * updating the UI in response to user actions.
     */
    class KCM final : public KCModule {
    Q_OBJECT

    public:
#if (QT_VERSION_MAJOR >= 6)
        /**
         * @brief Constructor for Qt 6 and above.
         * @param parent The parent object.
         * @param args Plugin metadata.
         */
        explicit KCM(QObject *parent, const KPluginMetaData &args);
#else
        /**
         * @brief Constructor for Qt 5.
         * @param parent The parent widget.
         * @param args Additional arguments.
         */
        explicit KCM(QWidget* parent = nullptr, const QVariantList& args = QVariantList());
#endif

    private Q_SLOTS:
        /**
         * @brief Resets the settings to their default values.
         */
        void defaults() override;

        /**
         * @brief Loads the current settings into the UI.
         */
        void load() override;

        /**
         * @brief Saves the current settings from the UI.
         */
        void save() override;

        /**
         * @brief Updates the color-related UI elements.
         */
        void update_colors();

        /**
         * @brief Updates the list of currently open windows in the UI.
         */
        void update_windows() const;

        /**
         * @brief Handles toggling of the outline group boxes.
         * @param value Whether the group is toggled on or off.
         */
        void outline_group_toggled(bool value) const;

    private:
        /**
         * @brief Pointer to the UI form.
         */
        std::shared_ptr<Ui::Form> ui;

        /**
         * @brief Configuration object for storing settings.
         */
        Config config;

        /**
         * @brief Loads the UI elements from the configuration.
         */
        void load_ui() const;

#if (QT_VERSION_MAJOR >= 6)
        /**
         * @brief Returns the palette used by the widget.
         * @return Reference to the widget's palette.
         */
        const QPalette& palette() { return widget()->palette(); };
#endif
    };
}
