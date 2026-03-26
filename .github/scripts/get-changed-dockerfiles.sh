#!/bin/bash
# Get the list of changed Dockerfiles
# Usage: get-changed-dockerfiles.sh <before-SHA> <head-SHA>

set -euxo pipefail

mapfile -t ALL_DOCKERFILES < <(git ls-files 'docker/Dockerfile.*')
if [[ -n "${BUILD_DOCKER_MANUAL_ENV:-}" ]]
then

    if [[ "${BUILD_DOCKER_MANUAL_ENV}" == "ALL" ]]
    then
        DOCKERFILES=("${ALL_DOCKERFILES[@]}")
    else
        BUILD_LIST=($BUILD_DOCKER_MANUAL_ENV)
        DOCKERFILES=()
        for item in "${BUILD_LIST[@]}"
        do
            if [[ -f "docker/Dockerfile.${item}" ]]
            then
                DOCKERFILES+=("$item")
            fi
        done
    fi
else
    CHANGED_FILES=$(git diff --name-only --diff-filter=ACMR HEAD^..HEAD)

    mapfile -t DOCKERFILES < <(echo "$CHANGED_FILES" | grep 'docker/Dockerfile')
    mapfile -t SCRIPT_CHANGES < <(echo "$CHANGED_FILES" | grep 'docker/' | grep '\.sh$')

    for file in "${SCRIPT_CHANGES[@]}"
    do
        for dockerfile in "${ALL_DOCKERFILES[@]}"
        do
            if grep -q "$(basename "$file")" "$dockerfile"
            then
                DOCKERFILES+=("$dockerfile")
            fi
        done
    done
fi

echo -n "matrix="
echo "${DOCKERFILES[@]}" | \
    tr ' ' '\n' | sort -u | sed 's,docker/Dockerfile\.,,' | \
    jq -Rsc 'split("\n") | map(select(length > 0)) | unique'
