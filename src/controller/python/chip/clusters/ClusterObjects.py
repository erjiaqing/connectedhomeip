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

from dataclasses import dataclass, asdict
from typing import List, Dict, Any, Mapping, Type
from chip import tlv
from dacite import from_dict


@dataclass
class ClusterObjectFieldDescriptor:
    Label: str
    Tag: int
    Type: Type


@dataclass
class ClusterObjectDescriptor:
    Fields: List[ClusterObjectFieldDescriptor]

    def GetFieldByTag(self, tag: int) -> ClusterObjectFieldDescriptor:
        for field in self.Fields:
            if field.Tag == tag:
                return field
        return None

    def GetFieldByLabel(self, label: str) -> ClusterObjectFieldDescriptor:
        for field in self.Fields:
            if field.Label == label:
                return field
        return None

    def _TagDictToLabelDict(self, path: List[int], tlvData: Dict[int, Any]) -> Dict[str, Any]:
        ret = {}
        for tag, value in tlvData.items():
            descriptor = self.GetFieldByTag(tag)
            if not descriptor:
                # We do not have enough infomation for this field.
                ret[tag] = value
                continue
            if not isinstance(descriptor.Type, ClusterObjectFieldDescriptor):
                if not isinstance(value, descriptor.Type):
                    raise Exception(
                        f"Failed to decode field {path + [tag]}, expected type {descriptor.Type}, got {type(value)}")
                ret[descriptor.Label] = value
                continue
            if isinstance(value, Mapping):
                raise Exception(
                    f"Failed to decode field {path + [tag]}, struct expected.")
            ret[descriptor.Label] = descriptor.Type._TagDictToLabelDict(
                self, path + [tag], value)
        return ret

    def TLVToDict(self, tlvBuf: bytes) -> Dict[str, Any]:
        tlvData = tlv.TLVReader(tlvBuf).get().get('Any')
        return self._TagDictToLabelDict([], tlvData)

    def _DictToTLV(self, path: List[str], tag, data: Mapping, writer: tlv.TLVWriter):
        writer.startStructure(tag)
        for field in self.Fields:
            val = data.get(field.Label, None)
            if not val:
                raise Exception(
                    f"Field {path + [field.Label]} is missing in the given dict")
            if isinstance(field.Type, ClusterObjectDescriptor):
                if not isinstance(val, dict):
                    raise Exception(
                        f"Field {path + [field.Label]} is a struct in TLV, {type(val)} given")
                self._DictToTLV(
                    path + [field.Label], field.Tag, val, writer)
                continue
            if not isinstance(val, field.Type):
                raise Exception(
                    f"Field {path + [field.Label]} is expecting type {field.Type}, {type(val)} given")
            writer.put(field.Tag, val)
        writer.endContainer()

    def DictToTLV(self, data: dict) -> bytes:
        tlvwriter = tlv.TLVWriter(bytearray())
        self._DictToTLV([], None, data, tlvwriter)
        return bytes(tlvwriter.encoding)


class ClusterObject:
    def ToTLV(self):
        return self.Descriptor.DictToTLV(asdict(self))

    @classmethod
    def FromDict(cls, data: dict):
        return from_dict(data_class=cls, data=data)

    @classmethod
    def FromTLV(cls, data: bytes):
        return cls.FromDict(data=cls.Descriptor.TLVToDict(data))
