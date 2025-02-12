#!/bin/bash

set -e

dbus-send --dest=org.kde.KWin --print-reply /Effects \
  org.kde.kwin.Effects.loadEffect string:kwin4_effect_shapecorners | awk 'NR==2 {print $2}'

kwriteconfig6 --file breezerc --group Common --key OutlineIntensity "OutlineOff"

# Find available qdbus binary
QDBUS_BIN=$(which qdbus6 qdbus-qt6 qdbus 2>/dev/null | head -n 1)

if [ -z "$QDBUS_BIN" ]; then
    echo "qdbus not found. Exiting."
    exit 1
fi

$QDBUS_BIN org.kde.KWin /KWin reconfigure
