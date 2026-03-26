Dockerfiles for CI builds
=========================

This repo contains a number of Dockerfiles for building libx52 against multiple
distros. Github only supports Ubuntu LTS for it's Linux runners, but libx52
users run multiple distros, including Arch Linux, Gentoo and Fedora.

For this reason, it's better to get a prebuilt Docker image that builds the CI
environment, pulling in all the relevant dependencies, and staging the CI build
environment as a container within Github. The regular build pipeline can then
run on these containers, and this will help ensure that the breakages are
reduced to a minimum.

Build Instructions
==================

In order to run the CI build on all the supported platforms, first run the
`build-containers.sh` script, it will find the Dockerfiles for all the
suppported platforms in the `docker` directory, and build the container images
for them with the necessary dependencies included.

Once the container images are built, you can build against one or all of the
container images by running `build-repo.sh`. If you don't specify the container
distro, which is basically the same as the extension after `Dockerfile.`, it
will run everything. If you specify a distro that doesn't exist, or has not been
built, the script will silently exit.

Extending to a new distro
=========================

To extend the builds to a new distro, create a `Dockerfile.<distro>` with the
necessary instructions to build a container image for that distro. Make sure you
install the necessary dependencies for the distro. It is strongly recommended to
add a `install-dependencies-<distro>.sh` so that you can build against multiple
versions of that distro, eg. Ubuntu 22.04 and Ubuntu 24.04. Make sure you copy
`ci-setup.sh` to allow setting up the environment, since it is highly likely
that the `meson dist` command will fail if you do not run that script.
