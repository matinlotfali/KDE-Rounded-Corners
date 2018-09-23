# KDE-Rounded-Corners

This is a fork of https://sourceforge.net/projects/shapecorners/

When you disable window borders in System Settings --> Application Style --> Window Decorations --> Border size: No Borders you get sharp bottom window corners.

This effect rounds the corners of your windows.


![After](https://raw.githubusercontent.com/alex47/KDE-Rounded-Corners/master/screenshots/after.PNG)

# How to build:
 - git clone https://github.com/alex47/KDE-Rounded-Corners
 - cd KDE-Rounded-Corners; mkdir qt5build; cd qt5build; cmake ../ -DCMAKE_INSTALL_PREFIX=/usr -DQT5BUILD=ON && make && sudo make install && (kwin_x11 --replace &)
