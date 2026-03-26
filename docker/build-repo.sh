#!/bin/bash
# Build the repository for all found directories
# Build containers for all the target environments
set -euo pipefail

GIT_ROOT=$(git rev-parse --show-toplevel)
CC=${CC:-gcc}

IMAGE="${1:-*}"

for image in $(docker images --filter "reference=ghcr.io/nirenjan/libx52/ci-build-${IMAGE}" --format '{{ .Repository}}')
do
    distro=${image##*/ci-build-}
    container_name="libx52-runner-${distro}"

    if [[ "$(docker ps -aq -f name=$container_name)" ]]
    then
        echo "Cleaning up old container for '$distro'"

        docker rm -f $container_name >/dev/null
    fi

    experimental=$(docker inspect --format='{{.Config.Labels}}' $image | \
        grep -q 'experimental:true' && echo " (experimental)" || true)

    if docker run --rm --name $container_name \
        --device /dev/bus/usb:/dev/bus/usb \
        -v "$GIT_ROOT":/code \
        -w /code \
        -e CC="${CC}" \
        $image \
        /bin/bash -c ".github/scripts/build-and-test.sh builddir/${distro}" \
        &> "$GIT_ROOT/build-${distro}.log"
    then
        echo "=== ${distro}${experimental} OK ==="
    else
        echo "=== ${distro}${experimental} !!! FAIL !!! ==="
        tail -20 "$GIT_ROOT/build-${distro}.log"
    fi
done
