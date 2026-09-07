#!/bin/sh
# Print a single sha256 over the file contents of one or more .deb/.rpm packages.
# Package metadata (build timestamps, version strings, dependency fields) and the
# package file names are not part of the hash, so a rebuild whose payload did not
# change hashes the same.
# Usage: ./package-hash.sh <package>...
# Requires dpkg-deb for .deb and rpm2cpio + cpio for .rpm.

set -e

if [ $# -eq 0 ]; then
    echo "Usage: $0 <package>..." >&2
    exit 1
fi

WORK_DIR=$(mktemp -d)
trap 'rm -rf "$WORK_DIR"' EXIT

for PKG in "$@"; do
    rm -rf "$WORK_DIR/pkg"
    mkdir "$WORK_DIR/pkg"
    case "$PKG" in
        *.deb) dpkg-deb -x "$PKG" "$WORK_DIR/pkg" ;;
        *.rpm) rpm2cpio "$PKG" | (cd "$WORK_DIR/pkg" && cpio -idm --quiet) ;;
        *) echo "Unsupported package format: $PKG" >&2; exit 1 ;;
    esac
    if [ -z "$(find "$WORK_DIR/pkg" -type f | head -n 1)" ]; then
        echo "No files extracted from $PKG" >&2
        exit 1
    fi
    # One line per package: hash of its file tree, independent of the package file name.
    (cd "$WORK_DIR/pkg" && find . -type f -exec sha256sum {} + | LC_ALL=C sort | sha256sum | cut -d' ' -f1) >> "$WORK_DIR/hashes"
done

LC_ALL=C sort "$WORK_DIR/hashes" | sha256sum | cut -d' ' -f1
