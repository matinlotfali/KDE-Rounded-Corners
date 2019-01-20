# KDE-Rounded-Corners

This is a fork of https://sourceforge.net/projects/shapecorners/

When you disable window borders in System Settings --> Application Style --> Window Decorations --> Border size: No Borders you get sharp bottom window corners.

This effect rounds the corners of your windows.


![After](https://raw.githubusercontent.com/alex47/KDE-Rounded-Corners/master/screenshots/after.PNG)

# Dependencies:
Debian based (Ubuntu, Kubuntu, KDE Neon):
```
sudo apt install git cmake g++ gettext extra-cmake-modules qttools5-dev libqt5x11extras5-dev libkf5configwidgets-dev libkf5crash-dev libkf5globalaccel-dev libkf5kio-dev libkf5notifications-dev kinit-dev kwin-dev 
```

# How to build:
```
git clone https://github.com/alex47/KDE-Rounded-Corners

cd KDE-Rounded-Corners; mkdir qt5build; cd qt5build; cmake ../ -DCMAKE_INSTALL_PREFIX=/usr -DQT5BUILD=ON && make && sudo make install && (kwin_x11 --replace &)
```


It should be now activated.

For better results turn off the border size in:

System Settings --> Application Style --> Window Decoration
