#!/bin/bash

set -e

REPO="$(cd "$(dirname "$0")/.." && pwd)"
BUILD="$REPO/build"
STAGE="/tmp/krc-nested/stage"
CFG="/tmp/krc-nested/config"

# Build the effect (configure first if the build dir is fresh)
cmake -S "$REPO" -B "$BUILD" -DCMAKE_BUILD_TYPE=Debug -DDEBUG_MAXIMIZED=1
cmake --build "$BUILD" -j"$(nproc)"

# Stage-install into a throwaway prefix so the nested session loads THIS build instead
# of the system-installed copy. Both lookups are overridden below: the plugin .so via
# QT_PLUGIN_PATH and the .frag shader via XDG_DATA_DIRS (it is loaded by path, not bundled).
rm -rf "$STAGE"
DESTDIR="$STAGE" cmake --install "$BUILD"

# Isolated kwinrc so the host Plasma config is untouched: enable the effect and turn
# squircles on with a visible radius. Adjust here or via System Settings inside the session.
mkdir -p "$CFG"
cat > "$CFG/kwinrc" <<'EOF'
[Plugins]
shapecornersEnabled=true

[Round-Corners]

EOF

# Launch a nested kwin_wayland running the full Plasma shell, composited by this build.
# Two isolations are required:
#   --socket          gives the nested compositor its OWN Wayland socket; without it the
#                     default (wayland-0) collides with the host session and clients can't
#                     connect, leaving a black window.
#   dbus-run-session  gives the session its OWN D-Bus bus; otherwise the host's plasmashell
#                     already owns org.kde.plasmashell and the nested shell renders black.
QT_PLUGIN_PATH="$STAGE/usr/lib/qt6/plugins${QT_PLUGIN_PATH:+:$QT_PLUGIN_PATH}" \
XDG_DATA_DIRS="$STAGE/usr/share:$XDG_DATA_DIRS" \
XDG_CONFIG_HOME="$CFG" \
    dbus-run-session -- kwin_wayland --socket wayland-krc --width 1600 --height 1000 \
                        --no-lockscreen --no-global-shortcuts --xwayland plasmashell || true
