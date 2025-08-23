#!/bin/bash

set -e

dbus-send --dest=org.kde.KWin --print-reply /Effects \
  org.kde.kwin.Effects.unloadEffect string:kwin4_effect_shapecorners > /dev/null

# Find available qdbus binary
QDBUS_BIN=$(which qdbus6 qdbus-qt6 qdbus 2>/dev/null | head -n 1)

if [ -z "$QDBUS_BIN" ]; then
    echo "qdbus not found. Exiting."
    exit 1
fi
$QDBUS_BIN org.kde.KWin /KWin reconfigure
