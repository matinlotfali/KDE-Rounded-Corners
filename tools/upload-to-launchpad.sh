#!/bin/bash
# Upload pre-built .deb to Launchpad PPA, or check whether a version is already published.
# Usage:
#   Upload: ./upload-to-launchpad.sh <deb_file> <package_name> <version> <distro_codename> <ubuntu_series>
#   Check:  ./upload-to-launchpad.sh --check <package_name> <version> <distro_codename>
#           Exits 0 if the source version is already on the PPA, non-zero otherwise.
# Example: ./upload-to-launchpad.sh shapecorners.deb kwin-effect-roundcorners-neon 0.7.2.5-6.5.3 neon noble

set -e

PPA="ppa:matinlotfali/kde-rounded-corners"

# Returns 0 if <pkg_name> at version <pkg_version>~<distro>1 is already published on the PPA.
# Fail-open on API errors: callers proceed with upload when Launchpad is unreachable.
launchpad_version_present() {
    LP_PKG="$1"
    LP_VERSION="${2}~${3}1"
    LP_URL="https://api.launchpad.net/1.0/~matinlotfali/+archive/ubuntu/kde-rounded-corners?ws.op=getPublishedSources&source_name=${LP_PKG}&version=${LP_VERSION}&exact_match=true"
    LP_COUNT=$(curl -fsS --max-time 30 "$LP_URL" 2>/dev/null | jq '.entries | length' 2>/dev/null || echo 0)
    [ "$LP_COUNT" -gt 0 ]
}

# Check-only mode
if [ "${1:-}" = "--check" ]; then
    PACKAGE_NAME="$2"
    PKG_VERSION="$3"
    DISTRO_CODENAME="$4"
    if launchpad_version_present "$PACKAGE_NAME" "$PKG_VERSION" "$DISTRO_CODENAME"; then
        echo "Found ${PACKAGE_NAME} ${PKG_VERSION}~${DISTRO_CODENAME}1 on Launchpad PPA."
        exit 0
    else
        echo "Did NOT find ${PACKAGE_NAME} ${PKG_VERSION}~${DISTRO_CODENAME}1 on Launchpad PPA."
        exit 1
    fi
fi

DEB_FILE="$1"
PACKAGE_NAME="$2"
PKG_VERSION="$3"
DISTRO_CODENAME="$4"
UBUNTU_SERIES="$5"

if [ -z "$DEB_FILE" ] || [ -z "$PACKAGE_NAME" ] || [ -z "$PKG_VERSION" ] || [ -z "$DISTRO_CODENAME" ] || [ -z "$UBUNTU_SERIES" ]; then
    echo "Usage: $0 <deb_file> <package_name> <version> <distro_codename> <ubuntu_series>"
    echo "       $0 --check <package_name> <version> <distro_codename>"
    exit 1
fi

# Version format: <pkg_version>~<distro>1 (e.g., 0.7.2.5-6.5.3~neon1)
VERSION="${PKG_VERSION}~${DISTRO_CODENAME}1"

# Skip the upload if this exact source version is already published on the PPA.
# Avoids the 550 buildinfo conflict that Launchpad's FTP returns for re-uploads.
if launchpad_version_present "$PACKAGE_NAME" "$PKG_VERSION" "$DISTRO_CODENAME"; then
    echo "${PACKAGE_NAME} ${VERSION} is already on Launchpad PPA. Skipping upload."
    exit 0
fi

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

override_dh_shlibdeps:
	# Pre-built binary linked against Plasma libs that may not exist in the
	# build chroot; rely on target system having KDE Plasma installed.
EOF
chmod +x "$SRC_DIR/debian/rules"

# Write debian/copyright (DEP-5 header + project LICENSE text) to silence
# Launchpad's "No copyright file found" warning.
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cat > "$SRC_DIR/debian/copyright" <<EOF
Format: https://www.debian.org/doc/packaging-manuals/copyright-format/1.0/
Upstream-Name: kde-rounded-corners
Upstream-Contact: Matin Lotfaliei <matinlotfali@gmail.com>
Source: https://github.com/matinlotfali/KDE-Rounded-Corners

Files: *
Copyright: 2022-$(date +%Y) Matin Lotfaliei <matinlotfali@gmail.com>
License: GPL-3.0+

License: GPL-3.0+
 The full text of the GNU General Public License version 3 follows.
 On Debian systems it can also be found in /usr/share/common-licenses/GPL-3.

EOF
cat "$SCRIPT_DIR/../LICENSE" >> "$SRC_DIR/debian/copyright"

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

cd "$WORK_DIR"

# Strip the source-only buildinfo from the upload. Launchpad's PPA FTP intermittently
# rejects _source.buildinfo files with "550 Requested action not taken: internal server
# error", failing the entire upload. The buildinfo is purely informational for source-only
# uploads (no binary build environment to record), so dropping it is safe and unblocks
# uploads. Done before debsign so the changes-file signature covers the modified content.
CHANGES_FILE="${PACKAGE_NAME}_${VERSION}_source.changes"
BUILDINFO_FILE="${PACKAGE_NAME}_${VERSION}_source.buildinfo"
sed -i "/_source\\.buildinfo/d" "$CHANGES_FILE"
rm -f "$BUILDINFO_FILE"

# Sign the source package
debsign -k"${GPG_KEY_ID}" "$CHANGES_FILE"

# Upload to Launchpad. -u skips dput's local pre-upload GPG verification, which
# fails on Ubuntu 24.04's GPG 2.4 with "SignatureVerifyError: 0" even when the
# signature is valid. Launchpad re-verifies the signature on its end.
dput -u "$PPA" "$CHANGES_FILE"

echo "Upload complete!"
rm -rf "$WORK_DIR"
