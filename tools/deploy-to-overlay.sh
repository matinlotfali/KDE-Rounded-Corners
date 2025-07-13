#!/bin/bash
set -e

# Remove previously installed files (if any)
rm -rf ~/kde/usr/lib/qt6/plugins/kwin/effects/plugins/kwin4_effect_shapecorners.so
rm -rf ~/kde/usr/lib/qt6/plugins/kwin/effects/configs/kwin_shapecorners_config.so
rm -rf ~/kde/usr/share/kwin/shaders/shapecorners*.frag

# Create required directories
mkdir -p ~/kde/usr/lib/qt6/plugins/kwin/effects/plugins
mkdir -p ~/kde/usr/lib/qt6/plugins/kwin/effects/configs
mkdir -p ~/kde/usr/share/kwin/shaders
mkdir -p ~/kde/usr/lib/extension-release.d

# Copy built plugins and shader files into overlay
cp ../install-root/lib/plugins/kwin/effects/plugins/kwin4_effect_shapecorners.so \
  ~/kde/usr/lib/qt6/plugins/kwin/effects/plugins/

cp ../install-root/lib/plugins/kwin/effects/configs/kwin_shapecorners_config.so \
  ~/kde/usr/lib/qt6/plugins/kwin/effects/configs/

cp ../install-root/share/kwin/shaders/shapecorners*.frag \
  ~/kde/usr/share/kwin/shaders/

# Create extension-release descriptor
cp /usr/lib/os-release ~/kde/usr/lib/extension-release.d/extension-release.kde
sed -i 's%^ID=.*%ID=_any%' ~/kde/usr/lib/extension-release.d/extension-release.kde
sudo chown root:root ~/kde/usr/lib/extension-release.d/extension-release.kde

# Link and merge the extension
sudo mkdir -p /var/lib/extensions
sudo ln -sf ~/kde /var/lib/extensions/kde

sudo systemd-sysext unmerge || true
sudo systemd-sysext merge

# Clean up install-root
rm -rf ../install-root