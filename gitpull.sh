#!/usr/bin/bash

cd /data/openpilot
BRANCH=$(git rev-parse --abbrev-ref HEAD)
HASH=$(git rev-parse HEAD)
git fetch
REMOTE_HASH=$(git rev-parse --verify origin/$BRANCH)
git pull

if [ "$HASH" != "$REMOTE_HASH" ]; then
  if [ -f "/data/openpilot/prebuilt" ]; then
    pkill -f thermald
    rm -f /data/openpilot/prebuilt
  fi
  sleep 5s
fi
