#
#    Copyright (c) 2021 Project CHIP Authors
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

#
#    @file
#      Provides Python APIs for CHIP.
#

"""Provides Python APIs for CHIP."""

__all__ = ["IMDelegate", "EventPath", "EventDataElement"]

from dataclasses import dataclass
from typing import Optional, Any


@dataclass
class EventPath:
    EndpointId: Optional[int] = None
    ClusterId: Optional[int] = None
    EventId: Optional[int] = None

    def from_dict(self, dict):
        self.EndpointId = dict.get(1, None)
        self.ClusterId = dict.get(2, None)
        self.EventId = dict.get(3, None)
        return self


@dataclass
class EventDataElement:
    Path: EventPath = EventPath()
    PriorityLevel: int = 0
    Number: int = 0
    UTCTimestamp: Optional[int] = None
    SystemTimestamp: Optional[int] = None
    DeltaUTCTimestamp: Optional[int] = None
    DeltaSystemTimestamp: Optional[int] = None
    EventData: Any = None

    def from_dict(self, dict):
        self.Path.from_dict(dict.get(0, {}))
        self.PriorityLevel = dict.get(1)
        self.Number = dict.get(2)
        self.UTCTimestamp = dict.get(3, None)
        self.SystemTimestamp = dict.get(4, None)
        self.DeltaUTCTimestamp = dict.get(5, None)
        self.DeltaSystemTimestamp = dict.get(6, None)
        self.EventData = dict.get(7, None)
        return self
