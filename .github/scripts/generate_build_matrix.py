#!/usr/bin/env python3
"""Generate the list of distros as a build matrix"""

import pathlib
import json
import sys

class BuildMatrix:
    """Generate a build matrix for Github actions"""

    COMPILER = 'gcc'
    OS = 'ubuntu-latest'
    EXPERIMENTAL = False

    def __init__(self, image_prefix):
        self.matrix = []
        self.image_prefix = image_prefix
        self.get_distros()
        self.add_extra_builds()
        self.generate_output()

    def build_matrix_obj(self, distro, experimental, os=None, compiler=None, image=None):
        """Build the matrix object for the given distro"""
        matrix_obj = {
            'distro': distro,
            'experimental': experimental,
            'os': os or self.OS,
            'compiler': compiler or self.COMPILER,
        }

        if image is None:
            image = f"{self.image_prefix}/ci-build-{distro}:latest"

        matrix_obj['image'] = image

        return matrix_obj

    def get_distros(self):
        """Get the list of distros from the Dockerfiles"""
        for dockerfile in pathlib.Path('.').glob('docker/Dockerfile.*'):
            distro = dockerfile.suffix[1:]

            with open(dockerfile, encoding='utf-8') as dfd:
                experimental = 'experimental="true"' in dfd.read()

            self.matrix.append(self.build_matrix_obj(distro, experimental))

    def add_extra_builds(self):
        """Add manual canary builds that don't have a corresponding dockerfile"""
        canary_build = self.build_matrix_obj('ubuntu22', False, compiler='clang')
        self.matrix.append(canary_build)

        macos_build = self.build_matrix_obj('macos', True, os='macos-latest',
                                            compiler='clang', image='')
        self.matrix.append(macos_build)

    def generate_output(self):
        """Generate the output for github actions"""
        matrix_data = json.dumps(self.matrix)
        print(f"matrix={matrix_data}")

if __name__ == '__main__':
    BuildMatrix(sys.argv[1])
