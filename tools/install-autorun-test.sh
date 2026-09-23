#!/bin/sh

AUTOSTART_FILE=~/.config/autostart/test-install-kde-rounded-corners.desktop
GIT_PATH="$(cd -- "$(dirname -- "$0")/.." && pwd)"
mkdir -p ~/.config/autostart
cp "${GIT_PATH}"/tools/test-install.desktop "${AUTOSTART_FILE}"
sed -i -e "s|Exec=|Exec=/usr/bin/sh ${GIT_PATH}/|g" "${AUTOSTART_FILE}"
