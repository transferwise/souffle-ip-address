#!/usr/bin/env bash

# If detects we are in the devcontainer, then runs rest of the arguments directly.
# Otherwise runs them in the devcontainer through docker exec.

set -euo pipefail

# Note: -f /.dockerenv in itself is not enough, since GHA runs in a container by default,
# but not our devcontainer.
#
# See below on how the docker run will mount the workdir to /src, which we check here.
if [ "$(pwd)" == "/src" ]
then
    # In docker, execute directly
    "$@"
else
    # Not in docker, exec in the devcontainer.
    #
    # shellcheck disable=SC2086
    docker run --rm ${DOCKER_ARGS:-} -v "$(pwd)":/src -w /src transferwise/souffle-ip-address:devcontainer "$@"
fi
