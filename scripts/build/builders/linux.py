# Copyright (c) 2021 Project CHIP Authors
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import os
from enum import Enum, auto

from .gn import GnBuilder


class LinuxApp(Enum):
    ALL_CLUSTERS = auto()
    CHIP_TOOL = auto()
    THERMOSTAT = auto()

    def ExamplePath(self):
        if self == LinuxApp.ALL_CLUSTERS:
            return 'all-clusters-app/linux'
        elif self == LinuxApp.CHIP_TOOL:
            return 'chip-tool'
        elif self == LinuxApp.THERMOSTAT:
            return 'thermostat/linux'
        else:
            raise Exception('Unknown app type: %r' % self)

    def BinaryName(self):
        if self == LinuxApp.ALL_CLUSTERS:
            return 'chip-all-clusters-app'
        elif self == LinuxApp.CHIP_TOOL:
            return 'chip-tool'
        elif self == LinuxApp.THERMOSTAT:
            return 'thermostat-app'
        else:
            raise Exception('Unknown app type: %r' % self)


class LinuxCPU(Enum):
    ARM = auto()
    ARM64 = auto()
    X64 = auto()

    def TargetCpuName(self):
        if self == LinuxCPU.ARM:
            return 'arm'
        elif self == LinuxCPU.ARM64:
            return 'arm64'
        elif self == LinuxCPU.X64:
            return 'x64'
        else:
            raise Exception('Unknown board type: %r' % self)

    def SysrootEnvironName(self):
        return f'{self.TargetCpuName().upper()}_SYSROOT'

    def SystemTargetTriple(self):
        if self == LinuxCPU.ARM:
            return 'armv7-linux-gnueabihf'
        elif self == LinuxCPU.ARM64:
            return 'aarch64-linux-gnu'
        elif self == LinuxCPU.X64:
            return 'x86_64-linux-gnu'
        else:
            raise Exception('Unknown board type: %r' % self)


class LinuxBuilder(GnBuilder):

    def __init__(self, root, runner, output_prefix: str, app: LinuxApp, board: LinuxCPU):
        super(LinuxBuilder, self).__init__(
            root=os.path.join(root, 'examples', app.ExamplePath()),
            runner=runner,
            output_prefix=output_prefix)

        self.cpu = board
        self.gn_build_args = []
        if board.SysrootEnvironName() in os.environ:
            self.gn_build_args += [
                f'sysroot="{os.environ[board.SysrootEnvironName()]}"']
        self.gn_build_args += [f'system_libdir="lib/{board.SystemTargetTriple()}"',
                               f'target_cpu="{board.TargetCpuName()}"', 'target_os="linux"', 'is_clang=true']
        self.app_name = app.BinaryName()
        self.map_name = self.app_name + '.map'

    def build_outputs(self):
        return {
            self.app_name: os.path.join(self.output_dir, self.app_name),
            self.map_name: os.path.join(self.output_dir, self.map_name)
        }

    def SetIdentifier(self, platform: str, board: str, app: str):
        super(LinuxBuilder, self).SetIdentifier(
            self.cpu.SystemTargetTriple(), board, app)
