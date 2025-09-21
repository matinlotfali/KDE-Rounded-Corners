#!/bin/bash

set -e

docker buildx build \
    -t debian-kde \
    -f .vscode/Dockerfile .

x11docker \
    --name=debian-kde \
    --backend=docker \
    --desktop \
    --home \
    --dbus \
    --gpu \
    --quiet \
    --hostdisplay \
    --clipboard \
    --interactive \
    --scale 0.8 \
    debian-kde || true
