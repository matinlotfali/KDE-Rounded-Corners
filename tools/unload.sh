#!/bin/bash
dbus-send --dest=org.kde.KWin --print-reply /Effects \
  org.kde.kwin.Effects.unloadEffect string:kwin4_effect_shapecorners > /dev/null
