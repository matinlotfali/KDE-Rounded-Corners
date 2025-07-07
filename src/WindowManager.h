/**
 * @file WindowManager.h
 * @brief Declares the ShapeCorners::WindowManager class for managing KWin windows and menu bars.
 *
 * This file provides the WindowManager class, which tracks managed windows and menu bars,
 * handles window addition/removal, tiling, maximization checks, and exposes window info via D-Bus.
 */

#pragma once

#include <QObject>
#include <unordered_map>
#include <vector>

namespace KWin {
    class EffectWindow;
}

namespace ShapeCorners {
    class Window;

    /**
     * @brief Alias for a map of managed windows.
     * Maps KWin::EffectWindow pointers to ShapeCorners::Window pointers.
     */
    using WindowList = std::unordered_map<const KWin::EffectWindow*, Window*>;

    /**
     * @brief Alias for a vector of managed menus.
     */
    using MenuBarList = std::vector<const KWin::EffectWindow*>;

    /**
     * @class WindowManager
     * @brief Manages ShapeCorners windows and menu bars, and handles window-related events.
     *
     * Handles adding/removing windows, tracking menu bars, checking tiling and maximization,
     * and provides a D-Bus interface for querying window information.
     */
    class WindowManager final: public QObject {
        Q_OBJECT

    public:
        /**
         * @brief Constructs the WindowManager, registers D-Bus, and adds existing windows.
         */
        WindowManager();

        /**
         * @brief Finds a managed window by its EffectWindow pointer.
         * @param kwindow The EffectWindow to search for.
         * @return Pointer to the managed Window, or nullptr if not found.
         */
        Window* findWindow(const KWin::EffectWindow *kwindow);

        /**
         * @brief Checks and adds a window to management, or as a menu bar if it's a dock.
         * @param kwindow The EffectWindow to add.
         * 
         * @return True if a new managed window was added, false if:
         * 
         * - The window is a dock (added as a menu bar instead)
         * 
         * - The window has an empty class and caption
         * 
         * - The window matches a hardcoded exception (e.g., KWin, lockscreen, etc.)
         * 
         * - The window is already managed (duplicate)
         */
        bool addWindow(KWin::EffectWindow *kwindow);

    public Q_SLOTS:
        
        /**
         * @brief Returns a JSON string of all managed window titles and classes.
         * It is being used by the D-Bus interface.
         * @return JSON string of window info.
         */
        [[nodiscard]]
        QString get_window_titles() const;

    private Q_SLOTS:

        /**
         * @brief Handles removal of a window or menu bar.
         * @param kwindow The EffectWindow to remove.
         */
        void windowRemoved(KWin::EffectWindow *kwindow);

        /**
         * @brief Checks if a window is maximized by comparing its geometry to the screen region.
         * @param kwindow The EffectWindow to check.
         */
        void checkMaximized(KWin::EffectWindow *kwindow);

        /**
         * @brief Handles window resize events by updating tiling and maximized state.
         * @param kwindow The EffectWindow that was resized.
         * @param size The new geometry (unused).
         */
        void windowResized(KWin::EffectWindow *kwindow, const QRectF &size);

    private:
        /**
         * @brief Map of managed windows.
         */
        WindowList m_managed;

        /**
         * @brief List of managed menu bar windows.
         */
        MenuBarList m_menuBars;

        /**
         * @brief Register D-Bus service and object for external communication
         */
        void registerDBus();

        /**
         * @brief Checks and marks tiled windows using TileChecker for all screens.
         */
        void checkTiled();

        /**
         * @brief Returns the region of the screen excluding menu bars.
         * @param screen_geometry The geometry of the screen.
         * @return QRegion without menu bars.
         */
        [[nodiscard]] 
        QRegion getRegionWithoutMenus(const QRect& screen_geometry) const;

    };
}