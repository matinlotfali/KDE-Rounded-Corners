#!/bin/bash
# Upload pre-built .deb to Launchpad PPA
# Usage: ./upload-to-launchpad.sh <deb_file> <package_name> <version> <distro_codename> <ubuntu_series>
# Example: ./upload-to-launchpad.sh shapecorners.deb kwin-effect-roundcorners-neon 0.7.2.5-6.5.3 neon noble

set -e

DEB_FILE="$1"
PACKAGE_NAME="$2"
PKG_VERSION="$3"
DISTRO_CODENAME="$4"
UBUNTU_SERIES="$5"
PPA="ppa:matinlotfali/kde-rounded-corners"

if [ -z "$DEB_FILE" ] || [ -z "$PACKAGE_NAME" ] || [ -z "$PKG_VERSION" ] || [ -z "$DISTRO_CODENAME" ] || [ -z "$UBUNTU_SERIES" ]; then
    echo "Usage: $0 <deb_file> <package_name> <version> <distro_codename> <ubuntu_series>"
    exit 1
fi

# Version format: <pkg_version>~<distro>1 (e.g., 0.7.2.5-6.5.3~neon1)
VERSION="${PKG_VERSION}~${DISTRO_CODENAME}1"

echo "Creating source package for $PACKAGE_NAME version $VERSION..."

# Create working directory
WORK_DIR=$(mktemp -d)
SRC_DIR="$WORK_DIR/${PACKAGE_NAME}-${VERSION}"
mkdir -p "$SRC_DIR/debian"

# Copy the pre-built .deb
cp "$DEB_FILE" "$SRC_DIR/"
DEB_BASENAME=$(basename "$DEB_FILE")

# Create minimal debian/ files
cat > "$SRC_DIR/debian/control" <<EOF
Source: ${PACKAGE_NAME}
Section: kde
Priority: optional
Maintainer: Matin Lotfaliei <matinlotfali@gmail.com>
Build-Depends: debhelper-compat (= 13)
Standards-Version: 4.6.0

Package: ${PACKAGE_NAME}
Architecture: amd64
Depends: \${misc:Depends}
Description: KDE Rounded Corners effect
 This KWin effect rounds the corners of your windows and adds
 an outline around them.
 .
 This package is for ${DISTRO_CODENAME}.
EOF

cat > "$SRC_DIR/debian/rules" <<EOF
#!/usr/bin/make -f
%:
	dh \$@

override_dh_auto_build:
	# Pre-built binary, nothing to build

override_dh_auto_install:
	dpkg-deb -x ${DEB_BASENAME} debian/${PACKAGE_NAME}
EOF
chmod +x "$SRC_DIR/debian/rules"

cat > "$SRC_DIR/debian/changelog" <<EOF
${PACKAGE_NAME} (${VERSION}) ${UBUNTU_SERIES}; urgency=medium

  * Pre-built package for ${DISTRO_CODENAME}

 -- Matin Lotfaliei <matinlotfali@gmail.com>  $(date -R)
EOF

mkdir -p "$SRC_DIR/debian/source"
echo "3.0 (native)" > "$SRC_DIR/debian/source/format"

# Build source package (-d to skip dependency checks since we're wrapping pre-built binary)
cd "$SRC_DIR"
dpkg-buildpackage -S -sa -us -uc -d

# Sign the source package
cd "$WORK_DIR"
debsign -k"${GPG_KEY_ID}" "${PACKAGE_NAME}_${VERSION}_source.changes"

# Upload to Launchpad
dput "$PPA" "${PACKAGE_NAME}_${VERSION}_source.changes"

echo "Upload complete!"
rm -rf "$WORK_DIR"
