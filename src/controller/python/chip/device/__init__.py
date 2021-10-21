#
#    Copyright (c) 2020 Project CHIP Authors
#    All rights reserved.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.
#

from chip import exceptions as ChipExceptions
from chip.clusters import ClusterObjects, Command
import chip.ChipStack

import asyncio


class Device:
    def __init__(self, native_device_object, node_id: int, chip_stack: chip.ChipStack):
        self._device = native_device_object
        self._node_id = node_id
        self._chip_stack = chip_stack
        pass

    @property
    def node_id(self) -> int:
        return self._node_id

    def SendCommand(self, endpoint: int, payload: ClusterObjects.ClusterCommand, responseType: ClusterObjects.ClusterCommand = None):
        eventLoop = asyncio.get_running_loop()
        future = eventLoop.create_future()

        res = self._ChipStack.Call(
            lambda: Command.SendCommand(
                future, eventLoop, responseType, self._device, Command.CommandPath(
                    EndpointId=endpoint,
                    ClusterId=payload.cluster_id,
                    CommandId=payload.command_id,
                ), payload)
        )
        if res != 0:
            future.set_exception(self._ChipStack.ErrorToException(res))
        return future
