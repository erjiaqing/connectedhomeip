from typing import List
from dataclasses import dataclass
import chip.native as Native


@dataclass
class AdapterInfo:
    index: int
    address: str
    name: str
    alias: str
    powered_on: bool


def GetAdapters() -> List[AdapterInfo]:
    """Get a list of BLE adapters available on the system. """

    result = []
    nativeList = Native.Api.Ble.adapter_list_new()
    if nativeList == 0:
        raise Exception('Failed to get BLE adapter list')

    try:
        while Native.Api.Ble.adapter_list_next(nativeList):
            result.append(
                AdapterInfo(
                    index=Native.Api.Ble.adapter_list_get_index(nativeList),
                    address=Native.Api.Ble.adapter_list_get_address(nativeList).decode('utf8'),
                    name=Native.Api.Ble.adapter_list_get_name(nativeList).decode('utf8'),
                    alias=Native.Api.Ble.adapter_list_get_alias(nativeList).decode('utf8'),
                    powered_on=Native.Api.Ble.adapter_list_is_powered(nativeList),
                ))

    finally:
        Native.Api.Ble.adapter_list_delete(nativeList)

    return result
