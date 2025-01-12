#!/bin/bash

set -e

dbus-send --dest=org.kde.KWin --print-reply /Effects \
  org.kde.kwin.Effects.unloadEffect string:kwin4_effect_shapecorners > /dev/null

kwriteconfig6 --file breezerc --group Common --key OutlineIntensity "OutlineOff"
qdbus-qt6 org.kde.KWin /KWin reconfigure
