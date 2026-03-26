#!/bin/bash
# Build containers for all the target environments
set -euo pipefail

SCRIPT_DIR=$(dirname "$0")
if command -v realpath &>/dev/null
then
    SCRIPT_DIR=$(realpath "$SCRIPT_DIR")
fi

GITHUB_SCRIPTS_DIR="$SCRIPT_DIR/../.github/scripts"
if command -v realpath &>/dev/null
then
    GITHUB_SCRIPTS_DIR=$(realpath "$GITHUB_SCRIPTS_DIR")
fi

for dockerfile in "$SCRIPT_DIR"/[Dd]ockerfile.*
do
    if [[ -e "$dockerfile" ]]
    then
        SUFFIX="$(basename "$dockerfile" | cut -d. -f2)"
        TAG="ghcr.io/nirenjan/libx52/ci-build-${SUFFIX}:latest"
        docker build --tag "$TAG" -f "$dockerfile" "${SCRIPT_DIR}"
    fi
done
