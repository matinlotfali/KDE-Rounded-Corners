#!/bin/bash
set -e

# Create extension-release descriptor
mkdir -p ~/kde/usr/lib/extension-release.d
cp /usr/lib/os-release ~/kde/usr/lib/extension-release.d/extension-release.kde
sed -i 's%^ID=.*%ID=_any%' ~/kde/usr/lib/extension-release.d/extension-release.kde
sudo chown root:root ~/kde/usr/lib/extension-release.d/extension-release.kde

# Link and merge the extension
sudo mkdir -p /var/lib/extensions
sudo ln -sf ~/kde /var/lib/extensions/kde

sudo systemd-sysext unmerge || true
sudo systemd-sysext merge
