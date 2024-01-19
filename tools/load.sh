dbus-send --dest=org.kde.KWin --print-reply /Effects \
  org.kde.kwin.Effects.loadEffect string:kwin4_effect_shapecorners | awk 'NR==2 {print $2}'