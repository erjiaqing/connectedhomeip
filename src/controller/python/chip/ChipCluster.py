'''
/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
'''

import ctypes
from .ChipStack import *
from .ChipExceptions import *

__all__ = ["ChipCluster"]

class ChipCluster:
    def __init__(self, chipstack):
        self._ChipStack = chipstack

    def SendCommand(self, device: ctypes.c_void_p):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_SendCommand(device)
        )

    def ListClusters(self):
        return {
            "BarrierControl": {
                "BarrierControlGoToPercent": {
                    "percentOpen": "int",
                },
                "BarrierControlStop": {
                },
            },
            "Basic": {
                "ResetToFactoryDefaults": {
                },
            },
            "ColorControl": {
                "MoveColor": {
                    "rateX": "int",
                    "rateY": "int",
                    "optionsMask": "int",
                    "optionsOverride": "int",
                },
                "MoveColorTemperature": {
                    "moveMode": "int",
                    "rate": "int",
                    "colorTemperatureMinimum": "int",
                    "colorTemperatureMaximum": "int",
                    "optionsMask": "int",
                    "optionsOverride": "int",
                },
                "MoveHue": {
                    "moveMode": "int",
                    "rate": "int",
                    "optionsMask": "int",
                    "optionsOverride": "int",
                },
                "MoveSaturation": {
                    "moveMode": "int",
                    "rate": "int",
                    "optionsMask": "int",
                    "optionsOverride": "int",
                },
                "MoveToColor": {
                    "colorX": "int",
                    "colorY": "int",
                    "transitionTime": "int",
                    "optionsMask": "int",
                    "optionsOverride": "int",
                },
                "MoveToColorTemperature": {
                    "colorTemperature": "int",
                    "transitionTime": "int",
                    "optionsMask": "int",
                    "optionsOverride": "int",
                },
                "MoveToHue": {
                    "hue": "int",
                    "direction": "int",
                    "transitionTime": "int",
                    "optionsMask": "int",
                    "optionsOverride": "int",
                },
                "MoveToHueAndSaturation": {
                    "hue": "int",
                    "saturation": "int",
                    "transitionTime": "int",
                    "optionsMask": "int",
                    "optionsOverride": "int",
                },
                "MoveToSaturation": {
                    "saturation": "int",
                    "transitionTime": "int",
                    "optionsMask": "int",
                    "optionsOverride": "int",
                },
                "StepColor": {
                    "stepX": "int",
                    "stepY": "int",
                    "transitionTime": "int",
                    "optionsMask": "int",
                    "optionsOverride": "int",
                },
                "StepColorTemperature": {
                    "stepMode": "int",
                    "stepSize": "int",
                    "transitionTime": "int",
                    "colorTemperatureMinimum": "int",
                    "colorTemperatureMaximum": "int",
                    "optionsMask": "int",
                    "optionsOverride": "int",
                },
                "StepHue": {
                    "stepMode": "int",
                    "stepSize": "int",
                    "transitionTime": "int",
                    "optionsMask": "int",
                    "optionsOverride": "int",
                },
                "StepSaturation": {
                    "stepMode": "int",
                    "stepSize": "int",
                    "transitionTime": "int",
                    "optionsMask": "int",
                    "optionsOverride": "int",
                },
                "StopMoveStep": {
                    "optionsMask": "int",
                    "optionsOverride": "int",
                },
            },
            "DoorLock": {
                "ClearAllPins": {
                },
                "ClearAllRfids": {
                },
                "ClearHolidaySchedule": {
                    "scheduleId": "int",
                },
                "ClearPin": {
                    "userId": "int",
                },
                "ClearRfid": {
                    "userId": "int",
                },
                "ClearWeekdaySchedule": {
                    "scheduleId": "int",
                    "userId": "int",
                },
                "ClearYeardaySchedule": {
                    "scheduleId": "int",
                    "userId": "int",
                },
                "GetHolidaySchedule": {
                    "scheduleId": "int",
                },
                "GetLogRecord": {
                    "logIndex": "int",
                },
                "GetPin": {
                    "userId": "int",
                },
                "GetRfid": {
                    "userId": "int",
                },
                "GetUserType": {
                    "userId": "int",
                },
                "GetWeekdaySchedule": {
                    "scheduleId": "int",
                    "userId": "int",
                },
                "GetYeardaySchedule": {
                    "scheduleId": "int",
                    "userId": "int",
                },
                "LockDoor": {
                    "pin": "str",
                },
                "SetHolidaySchedule": {
                    "scheduleId": "int",
                    "localStartTime": "int",
                    "localEndTime": "int",
                    "operatingModeDuringHoliday": "int",
                },
                "SetPin": {
                    "userId": "int",
                    "userStatus": "int",
                    "userType": "int",
                    "pin": "str",
                },
                "SetRfid": {
                    "userId": "int",
                    "userStatus": "int",
                    "userType": "int",
                    "id": "str",
                },
                "SetUserType": {
                    "userId": "int",
                    "userType": "int",
                },
                "SetWeekdaySchedule": {
                    "scheduleId": "int",
                    "userId": "int",
                    "daysMask": "int",
                    "startHour": "int",
                    "startMinute": "int",
                    "endHour": "int",
                    "endMinute": "int",
                },
                "SetYeardaySchedule": {
                    "scheduleId": "int",
                    "userId": "int",
                    "localStartTime": "int",
                    "localEndTime": "int",
                },
                "UnlockDoor": {
                    "pin": "str",
                },
                "UnlockWithTimeout": {
                    "timeoutInSeconds": "int",
                    "pin": "str",
                },
            },
            "Groups": {
                "AddGroup": {
                    "groupId": "int",
                    "groupName": "str",
                },
                "AddGroupIfIdentifying": {
                    "groupId": "int",
                    "groupName": "str",
                },
                "GetGroupMembership": {
                    "groupCount": "int",
                    "groupList": "int",
                },
                "RemoveAllGroups": {
                },
                "RemoveGroup": {
                    "groupId": "int",
                },
                "ViewGroup": {
                    "groupId": "int",
                },
            },
            "IasZone": {
            },
            "Identify": {
                "Identify": {
                    "identifyTime": "int",
                },
                "IdentifyQuery": {
                },
            },
            "LevelControl": {
                "Move": {
                    "moveMode": "int",
                    "rate": "int",
                    "optionMask": "int",
                    "optionOverride": "int",
                },
                "MoveToLevel": {
                    "level": "int",
                    "transitionTime": "int",
                    "optionMask": "int",
                    "optionOverride": "int",
                },
                "MoveToLevelWithOnOff": {
                    "level": "int",
                    "transitionTime": "int",
                },
                "MoveWithOnOff": {
                    "moveMode": "int",
                    "rate": "int",
                },
                "Step": {
                    "stepMode": "int",
                    "stepSize": "int",
                    "transitionTime": "int",
                    "optionMask": "int",
                    "optionOverride": "int",
                },
                "StepWithOnOff": {
                    "stepMode": "int",
                    "stepSize": "int",
                    "transitionTime": "int",
                },
                "Stop": {
                    "optionMask": "int",
                    "optionOverride": "int",
                },
                "StopWithOnOff": {
                },
            },
            "OnOff": {
                "Off": {
                },
                "On": {
                },
                "Toggle": {
                },
            },
            "Scenes": {
                "AddScene": {
                    "groupId": "int",
                    "sceneId": "int",
                    "transitionTime": "int",
                    "sceneName": "str",
                    "clusterId": "int",
                    "length": "int",
                    "value": "int",
                },
                "GetSceneMembership": {
                    "groupId": "int",
                },
                "RecallScene": {
                    "groupId": "int",
                    "sceneId": "int",
                    "transitionTime": "int",
                },
                "RemoveAllScenes": {
                    "groupId": "int",
                },
                "RemoveScene": {
                    "groupId": "int",
                    "sceneId": "int",
                },
                "StoreScene": {
                    "groupId": "int",
                    "sceneId": "int",
                },
                "ViewScene": {
                    "groupId": "int",
                    "sceneId": "int",
                },
            },
            "TemperatureMeasurement": {
            },
        }

    def PrepareCommand(self, device: ctypes.c_void_p, cluster: str, command: str, endpoint: int, groupid: int, args):
        func = getattr(self, "Cluster{}_Command{}".format(cluster, command), None)
        if not func:
            raise UnknownCommand(cluster, command)
        func(endpoint, groupid, **args)

    def ClusterBarrierControl_CommandBarrierControlGoToPercent(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, percentOpen: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_BarrierControl_BarrierControlGoToPercent(
                device, ZCLendpoint, ZCLgroupid, percentOpen
            )
        )

    def ClusterBarrierControl_CommandBarrierControlStop(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_BarrierControl_BarrierControlStop(
                device, ZCLendpoint, ZCLgroupid
            )
        )

    def ClusterBasic_CommandResetToFactoryDefaults(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_Basic_ResetToFactoryDefaults(
                device, ZCLendpoint, ZCLgroupid
            )
        )

    def ClusterColorControl_CommandMoveColor(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, rateX: int, rateY: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_ColorControl_MoveColor(
                device, ZCLendpoint, ZCLgroupid, rateX, rateY, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandMoveColorTemperature(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, moveMode: int, rate: int, colorTemperatureMinimum: int, colorTemperatureMaximum: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_ColorControl_MoveColorTemperature(
                device, ZCLendpoint, ZCLgroupid, moveMode, rate, colorTemperatureMinimum, colorTemperatureMaximum, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandMoveHue(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, moveMode: int, rate: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_ColorControl_MoveHue(
                device, ZCLendpoint, ZCLgroupid, moveMode, rate, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandMoveSaturation(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, moveMode: int, rate: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_ColorControl_MoveSaturation(
                device, ZCLendpoint, ZCLgroupid, moveMode, rate, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandMoveToColor(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, colorX: int, colorY: int, transitionTime: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_ColorControl_MoveToColor(
                device, ZCLendpoint, ZCLgroupid, colorX, colorY, transitionTime, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandMoveToColorTemperature(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, colorTemperature: int, transitionTime: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_ColorControl_MoveToColorTemperature(
                device, ZCLendpoint, ZCLgroupid, colorTemperature, transitionTime, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandMoveToHue(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, hue: int, direction: int, transitionTime: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_ColorControl_MoveToHue(
                device, ZCLendpoint, ZCLgroupid, hue, direction, transitionTime, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandMoveToHueAndSaturation(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, hue: int, saturation: int, transitionTime: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_ColorControl_MoveToHueAndSaturation(
                device, ZCLendpoint, ZCLgroupid, hue, saturation, transitionTime, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandMoveToSaturation(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, saturation: int, transitionTime: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_ColorControl_MoveToSaturation(
                device, ZCLendpoint, ZCLgroupid, saturation, transitionTime, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandStepColor(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, stepX: int, stepY: int, transitionTime: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_ColorControl_StepColor(
                device, ZCLendpoint, ZCLgroupid, stepX, stepY, transitionTime, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandStepColorTemperature(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, stepMode: int, stepSize: int, transitionTime: int, colorTemperatureMinimum: int, colorTemperatureMaximum: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_ColorControl_StepColorTemperature(
                device, ZCLendpoint, ZCLgroupid, stepMode, stepSize, transitionTime, colorTemperatureMinimum, colorTemperatureMaximum, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandStepHue(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, stepMode: int, stepSize: int, transitionTime: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_ColorControl_StepHue(
                device, ZCLendpoint, ZCLgroupid, stepMode, stepSize, transitionTime, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandStepSaturation(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, stepMode: int, stepSize: int, transitionTime: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_ColorControl_StepSaturation(
                device, ZCLendpoint, ZCLgroupid, stepMode, stepSize, transitionTime, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandStopMoveStep(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_ColorControl_StopMoveStep(
                device, ZCLendpoint, ZCLgroupid, optionsMask, optionsOverride
            )
        )

    def ClusterDoorLock_CommandClearAllPins(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_ClearAllPins(
                device, ZCLendpoint, ZCLgroupid
            )
        )

    def ClusterDoorLock_CommandClearAllRfids(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_ClearAllRfids(
                device, ZCLendpoint, ZCLgroupid
            )
        )

    def ClusterDoorLock_CommandClearHolidaySchedule(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, scheduleId: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_ClearHolidaySchedule(
                device, ZCLendpoint, ZCLgroupid, scheduleId
            )
        )

    def ClusterDoorLock_CommandClearPin(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, userId: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_ClearPin(
                device, ZCLendpoint, ZCLgroupid, userId
            )
        )

    def ClusterDoorLock_CommandClearRfid(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, userId: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_ClearRfid(
                device, ZCLendpoint, ZCLgroupid, userId
            )
        )

    def ClusterDoorLock_CommandClearWeekdaySchedule(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, scheduleId: int, userId: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_ClearWeekdaySchedule(
                device, ZCLendpoint, ZCLgroupid, scheduleId, userId
            )
        )

    def ClusterDoorLock_CommandClearYeardaySchedule(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, scheduleId: int, userId: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_ClearYeardaySchedule(
                device, ZCLendpoint, ZCLgroupid, scheduleId, userId
            )
        )

    def ClusterDoorLock_CommandGetHolidaySchedule(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, scheduleId: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_GetHolidaySchedule(
                device, ZCLendpoint, ZCLgroupid, scheduleId
            )
        )

    def ClusterDoorLock_CommandGetLogRecord(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, logIndex: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_GetLogRecord(
                device, ZCLendpoint, ZCLgroupid, logIndex
            )
        )

    def ClusterDoorLock_CommandGetPin(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, userId: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_GetPin(
                device, ZCLendpoint, ZCLgroupid, userId
            )
        )

    def ClusterDoorLock_CommandGetRfid(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, userId: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_GetRfid(
                device, ZCLendpoint, ZCLgroupid, userId
            )
        )

    def ClusterDoorLock_CommandGetUserType(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, userId: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_GetUserType(
                device, ZCLendpoint, ZCLgroupid, userId
            )
        )

    def ClusterDoorLock_CommandGetWeekdaySchedule(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, scheduleId: int, userId: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_GetWeekdaySchedule(
                device, ZCLendpoint, ZCLgroupid, scheduleId, userId
            )
        )

    def ClusterDoorLock_CommandGetYeardaySchedule(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, scheduleId: int, userId: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_GetYeardaySchedule(
                device, ZCLendpoint, ZCLgroupid, scheduleId, userId
            )
        )

    def ClusterDoorLock_CommandLockDoor(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, pin: str):
        pin = pin.encode("utf-8") + b'\x00'
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_LockDoor(
                device, ZCLendpoint, ZCLgroupid, pin
            )
        )

    def ClusterDoorLock_CommandSetHolidaySchedule(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, scheduleId: int, localStartTime: int, localEndTime: int, operatingModeDuringHoliday: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_SetHolidaySchedule(
                device, ZCLendpoint, ZCLgroupid, scheduleId, localStartTime, localEndTime, operatingModeDuringHoliday
            )
        )

    def ClusterDoorLock_CommandSetPin(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, userId: int, userStatus: int, userType: int, pin: str):
        pin = pin.encode("utf-8") + b'\x00'
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_SetPin(
                device, ZCLendpoint, ZCLgroupid, userId, userStatus, userType, pin
            )
        )

    def ClusterDoorLock_CommandSetRfid(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, userId: int, userStatus: int, userType: int, id: str):
        id = id.encode("utf-8") + b'\x00'
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_SetRfid(
                device, ZCLendpoint, ZCLgroupid, userId, userStatus, userType, id
            )
        )

    def ClusterDoorLock_CommandSetUserType(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, userId: int, userType: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_SetUserType(
                device, ZCLendpoint, ZCLgroupid, userId, userType
            )
        )

    def ClusterDoorLock_CommandSetWeekdaySchedule(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, scheduleId: int, userId: int, daysMask: int, startHour: int, startMinute: int, endHour: int, endMinute: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_SetWeekdaySchedule(
                device, ZCLendpoint, ZCLgroupid, scheduleId, userId, daysMask, startHour, startMinute, endHour, endMinute
            )
        )

    def ClusterDoorLock_CommandSetYeardaySchedule(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, scheduleId: int, userId: int, localStartTime: int, localEndTime: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_SetYeardaySchedule(
                device, ZCLendpoint, ZCLgroupid, scheduleId, userId, localStartTime, localEndTime
            )
        )

    def ClusterDoorLock_CommandUnlockDoor(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, pin: str):
        pin = pin.encode("utf-8") + b'\x00'
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_UnlockDoor(
                device, ZCLendpoint, ZCLgroupid, pin
            )
        )

    def ClusterDoorLock_CommandUnlockWithTimeout(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, timeoutInSeconds: int, pin: str):
        pin = pin.encode("utf-8") + b'\x00'
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_DoorLock_UnlockWithTimeout(
                device, ZCLendpoint, ZCLgroupid, timeoutInSeconds, pin
            )
        )

    def ClusterGroups_CommandAddGroup(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, groupId: int, groupName: str):
        groupName = groupName.encode("utf-8") + b'\x00'
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_Groups_AddGroup(
                device, ZCLendpoint, ZCLgroupid, groupId, groupName
            )
        )

    def ClusterGroups_CommandAddGroupIfIdentifying(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, groupId: int, groupName: str):
        groupName = groupName.encode("utf-8") + b'\x00'
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_Groups_AddGroupIfIdentifying(
                device, ZCLendpoint, ZCLgroupid, groupId, groupName
            )
        )

    def ClusterGroups_CommandGetGroupMembership(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, groupCount: int, groupList: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_Groups_GetGroupMembership(
                device, ZCLendpoint, ZCLgroupid, groupCount, groupList
            )
        )

    def ClusterGroups_CommandRemoveAllGroups(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_Groups_RemoveAllGroups(
                device, ZCLendpoint, ZCLgroupid
            )
        )

    def ClusterGroups_CommandRemoveGroup(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, groupId: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_Groups_RemoveGroup(
                device, ZCLendpoint, ZCLgroupid, groupId
            )
        )

    def ClusterGroups_CommandViewGroup(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, groupId: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_Groups_ViewGroup(
                device, ZCLendpoint, ZCLgroupid, groupId
            )
        )

    def ClusterIdentify_CommandIdentify(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, identifyTime: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_Identify_Identify(
                device, ZCLendpoint, ZCLgroupid, identifyTime
            )
        )

    def ClusterIdentify_CommandIdentifyQuery(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_Identify_IdentifyQuery(
                device, ZCLendpoint, ZCLgroupid
            )
        )

    def ClusterLevelControl_CommandMove(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, moveMode: int, rate: int, optionMask: int, optionOverride: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_LevelControl_Move(
                device, ZCLendpoint, ZCLgroupid, moveMode, rate, optionMask, optionOverride
            )
        )

    def ClusterLevelControl_CommandMoveToLevel(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, level: int, transitionTime: int, optionMask: int, optionOverride: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_LevelControl_MoveToLevel(
                device, ZCLendpoint, ZCLgroupid, level, transitionTime, optionMask, optionOverride
            )
        )

    def ClusterLevelControl_CommandMoveToLevelWithOnOff(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, level: int, transitionTime: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_LevelControl_MoveToLevelWithOnOff(
                device, ZCLendpoint, ZCLgroupid, level, transitionTime
            )
        )

    def ClusterLevelControl_CommandMoveWithOnOff(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, moveMode: int, rate: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_LevelControl_MoveWithOnOff(
                device, ZCLendpoint, ZCLgroupid, moveMode, rate
            )
        )

    def ClusterLevelControl_CommandStep(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, stepMode: int, stepSize: int, transitionTime: int, optionMask: int, optionOverride: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_LevelControl_Step(
                device, ZCLendpoint, ZCLgroupid, stepMode, stepSize, transitionTime, optionMask, optionOverride
            )
        )

    def ClusterLevelControl_CommandStepWithOnOff(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, stepMode: int, stepSize: int, transitionTime: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_LevelControl_StepWithOnOff(
                device, ZCLendpoint, ZCLgroupid, stepMode, stepSize, transitionTime
            )
        )

    def ClusterLevelControl_CommandStop(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, optionMask: int, optionOverride: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_LevelControl_Stop(
                device, ZCLendpoint, ZCLgroupid, optionMask, optionOverride
            )
        )

    def ClusterLevelControl_CommandStopWithOnOff(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_LevelControl_StopWithOnOff(
                device, ZCLendpoint, ZCLgroupid
            )
        )

    def ClusterOnOff_CommandOff(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_OnOff_Off(
                device, ZCLendpoint, ZCLgroupid
            )
        )

    def ClusterOnOff_CommandOn(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_OnOff_On(
                device, ZCLendpoint, ZCLgroupid
            )
        )

    def ClusterOnOff_CommandToggle(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_OnOff_Toggle(
                device, ZCLendpoint, ZCLgroupid
            )
        )

    def ClusterScenes_CommandAddScene(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, groupId: int, sceneId: int, transitionTime: int, sceneName: str, clusterId: int, length: int, value: int):
        sceneName = sceneName.encode("utf-8") + b'\x00'
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_Scenes_AddScene(
                device, ZCLendpoint, ZCLgroupid, groupId, sceneId, transitionTime, sceneName, clusterId, length, value
            )
        )

    def ClusterScenes_CommandGetSceneMembership(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, groupId: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_Scenes_GetSceneMembership(
                device, ZCLendpoint, ZCLgroupid, groupId
            )
        )

    def ClusterScenes_CommandRecallScene(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, groupId: int, sceneId: int, transitionTime: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_Scenes_RecallScene(
                device, ZCLendpoint, ZCLgroupid, groupId, sceneId, transitionTime
            )
        )

    def ClusterScenes_CommandRemoveAllScenes(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, groupId: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_Scenes_RemoveAllScenes(
                device, ZCLendpoint, ZCLgroupid, groupId
            )
        )

    def ClusterScenes_CommandRemoveScene(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, groupId: int, sceneId: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_Scenes_RemoveScene(
                device, ZCLendpoint, ZCLgroupid, groupId, sceneId
            )
        )

    def ClusterScenes_CommandStoreScene(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, groupId: int, sceneId: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_Scenes_StoreScene(
                device, ZCLendpoint, ZCLgroupid, groupId, sceneId
            )
        )

    def ClusterScenes_CommandViewScene(self, device: ctypes.c_void_p, ZCLendpoint: int, ZCLgroupid: int, groupId: int, sceneId: int):
        self._ChipStack.Call(
            lambda: self._chipLib.chip_ime_AppendCommand_Scenes_ViewScene(
                device, ZCLendpoint, ZCLgroupid, groupId, sceneId
            )
        )

    def InitLib(self, chipLib):
        self._chipLib = chipLib
        self._chipLib.chip_ime_SendCommand.argtypes = [ctypes.c_void_p, ctypes.c_uint64]
        self._chipLib.chip_ime_SendCommand.restype = ctypes.c_uint32
        # Cluster BarrierControl
        # Cluster BarrierControl Command BarrierControlGoToPercent
        self._chipLib.chip_ime_AppendCommand_BarrierControl_BarrierControlGoToPercent.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_BarrierControl_BarrierControlGoToPercent.restype = ctypes.c_uint32
        # Cluster BarrierControl Command BarrierControlStop
        self._chipLib.chip_ime_AppendCommand_BarrierControl_BarrierControlStop.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_BarrierControl_BarrierControlStop.restype = ctypes.c_uint32
        # Cluster Basic
        # Cluster Basic Command ResetToFactoryDefaults
        self._chipLib.chip_ime_AppendCommand_Basic_ResetToFactoryDefaults.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_Basic_ResetToFactoryDefaults.restype = ctypes.c_uint32
        # Cluster ColorControl
        # Cluster ColorControl Command MoveColor
        self._chipLib.chip_ime_AppendCommand_ColorControl_MoveColor.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_int16, ctypes.c_int16, ctypes.c_uint8, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_ColorControl_MoveColor.restype = ctypes.c_uint32
        # Cluster ColorControl Command MoveColorTemperature
        self._chipLib.chip_ime_AppendCommand_ColorControl_MoveColorTemperature.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_ColorControl_MoveColorTemperature.restype = ctypes.c_uint32
        # Cluster ColorControl Command MoveHue
        self._chipLib.chip_ime_AppendCommand_ColorControl_MoveHue.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_ColorControl_MoveHue.restype = ctypes.c_uint32
        # Cluster ColorControl Command MoveSaturation
        self._chipLib.chip_ime_AppendCommand_ColorControl_MoveSaturation.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_ColorControl_MoveSaturation.restype = ctypes.c_uint32
        # Cluster ColorControl Command MoveToColor
        self._chipLib.chip_ime_AppendCommand_ColorControl_MoveToColor.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_ColorControl_MoveToColor.restype = ctypes.c_uint32
        # Cluster ColorControl Command MoveToColorTemperature
        self._chipLib.chip_ime_AppendCommand_ColorControl_MoveToColorTemperature.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_ColorControl_MoveToColorTemperature.restype = ctypes.c_uint32
        # Cluster ColorControl Command MoveToHue
        self._chipLib.chip_ime_AppendCommand_ColorControl_MoveToHue.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_ColorControl_MoveToHue.restype = ctypes.c_uint32
        # Cluster ColorControl Command MoveToHueAndSaturation
        self._chipLib.chip_ime_AppendCommand_ColorControl_MoveToHueAndSaturation.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_ColorControl_MoveToHueAndSaturation.restype = ctypes.c_uint32
        # Cluster ColorControl Command MoveToSaturation
        self._chipLib.chip_ime_AppendCommand_ColorControl_MoveToSaturation.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_ColorControl_MoveToSaturation.restype = ctypes.c_uint32
        # Cluster ColorControl Command StepColor
        self._chipLib.chip_ime_AppendCommand_ColorControl_StepColor.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_int16, ctypes.c_int16, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_ColorControl_StepColor.restype = ctypes.c_uint32
        # Cluster ColorControl Command StepColorTemperature
        self._chipLib.chip_ime_AppendCommand_ColorControl_StepColorTemperature.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_ColorControl_StepColorTemperature.restype = ctypes.c_uint32
        # Cluster ColorControl Command StepHue
        self._chipLib.chip_ime_AppendCommand_ColorControl_StepHue.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_ColorControl_StepHue.restype = ctypes.c_uint32
        # Cluster ColorControl Command StepSaturation
        self._chipLib.chip_ime_AppendCommand_ColorControl_StepSaturation.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_ColorControl_StepSaturation.restype = ctypes.c_uint32
        # Cluster ColorControl Command StopMoveStep
        self._chipLib.chip_ime_AppendCommand_ColorControl_StopMoveStep.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_ColorControl_StopMoveStep.restype = ctypes.c_uint32
        # Cluster DoorLock
        # Cluster DoorLock Command ClearAllPins
        self._chipLib.chip_ime_AppendCommand_DoorLock_ClearAllPins.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_DoorLock_ClearAllPins.restype = ctypes.c_uint32
        # Cluster DoorLock Command ClearAllRfids
        self._chipLib.chip_ime_AppendCommand_DoorLock_ClearAllRfids.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_DoorLock_ClearAllRfids.restype = ctypes.c_uint32
        # Cluster DoorLock Command ClearHolidaySchedule
        self._chipLib.chip_ime_AppendCommand_DoorLock_ClearHolidaySchedule.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_DoorLock_ClearHolidaySchedule.restype = ctypes.c_uint32
        # Cluster DoorLock Command ClearPin
        self._chipLib.chip_ime_AppendCommand_DoorLock_ClearPin.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_DoorLock_ClearPin.restype = ctypes.c_uint32
        # Cluster DoorLock Command ClearRfid
        self._chipLib.chip_ime_AppendCommand_DoorLock_ClearRfid.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_DoorLock_ClearRfid.restype = ctypes.c_uint32
        # Cluster DoorLock Command ClearWeekdaySchedule
        self._chipLib.chip_ime_AppendCommand_DoorLock_ClearWeekdaySchedule.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_DoorLock_ClearWeekdaySchedule.restype = ctypes.c_uint32
        # Cluster DoorLock Command ClearYeardaySchedule
        self._chipLib.chip_ime_AppendCommand_DoorLock_ClearYeardaySchedule.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_DoorLock_ClearYeardaySchedule.restype = ctypes.c_uint32
        # Cluster DoorLock Command GetHolidaySchedule
        self._chipLib.chip_ime_AppendCommand_DoorLock_GetHolidaySchedule.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_DoorLock_GetHolidaySchedule.restype = ctypes.c_uint32
        # Cluster DoorLock Command GetLogRecord
        self._chipLib.chip_ime_AppendCommand_DoorLock_GetLogRecord.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_DoorLock_GetLogRecord.restype = ctypes.c_uint32
        # Cluster DoorLock Command GetPin
        self._chipLib.chip_ime_AppendCommand_DoorLock_GetPin.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_DoorLock_GetPin.restype = ctypes.c_uint32
        # Cluster DoorLock Command GetRfid
        self._chipLib.chip_ime_AppendCommand_DoorLock_GetRfid.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_DoorLock_GetRfid.restype = ctypes.c_uint32
        # Cluster DoorLock Command GetUserType
        self._chipLib.chip_ime_AppendCommand_DoorLock_GetUserType.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_DoorLock_GetUserType.restype = ctypes.c_uint32
        # Cluster DoorLock Command GetWeekdaySchedule
        self._chipLib.chip_ime_AppendCommand_DoorLock_GetWeekdaySchedule.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_DoorLock_GetWeekdaySchedule.restype = ctypes.c_uint32
        # Cluster DoorLock Command GetYeardaySchedule
        self._chipLib.chip_ime_AppendCommand_DoorLock_GetYeardaySchedule.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_DoorLock_GetYeardaySchedule.restype = ctypes.c_uint32
        # Cluster DoorLock Command LockDoor
        self._chipLib.chip_ime_AppendCommand_DoorLock_LockDoor.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_char_p]
        self._chipLib.chip_ime_AppendCommand_DoorLock_LockDoor.restype = ctypes.c_uint32
        # Cluster DoorLock Command SetHolidaySchedule
        self._chipLib.chip_ime_AppendCommand_DoorLock_SetHolidaySchedule.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint32, ctypes.c_uint32, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_DoorLock_SetHolidaySchedule.restype = ctypes.c_uint32
        # Cluster DoorLock Command SetPin
        self._chipLib.chip_ime_AppendCommand_DoorLock_SetPin.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_char_p]
        self._chipLib.chip_ime_AppendCommand_DoorLock_SetPin.restype = ctypes.c_uint32
        # Cluster DoorLock Command SetRfid
        self._chipLib.chip_ime_AppendCommand_DoorLock_SetRfid.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_char_p]
        self._chipLib.chip_ime_AppendCommand_DoorLock_SetRfid.restype = ctypes.c_uint32
        # Cluster DoorLock Command SetUserType
        self._chipLib.chip_ime_AppendCommand_DoorLock_SetUserType.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_DoorLock_SetUserType.restype = ctypes.c_uint32
        # Cluster DoorLock Command SetWeekdaySchedule
        self._chipLib.chip_ime_AppendCommand_DoorLock_SetWeekdaySchedule.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_DoorLock_SetWeekdaySchedule.restype = ctypes.c_uint32
        # Cluster DoorLock Command SetYeardaySchedule
        self._chipLib.chip_ime_AppendCommand_DoorLock_SetYeardaySchedule.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint32, ctypes.c_uint32]
        self._chipLib.chip_ime_AppendCommand_DoorLock_SetYeardaySchedule.restype = ctypes.c_uint32
        # Cluster DoorLock Command UnlockDoor
        self._chipLib.chip_ime_AppendCommand_DoorLock_UnlockDoor.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_char_p]
        self._chipLib.chip_ime_AppendCommand_DoorLock_UnlockDoor.restype = ctypes.c_uint32
        # Cluster DoorLock Command UnlockWithTimeout
        self._chipLib.chip_ime_AppendCommand_DoorLock_UnlockWithTimeout.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_char_p]
        self._chipLib.chip_ime_AppendCommand_DoorLock_UnlockWithTimeout.restype = ctypes.c_uint32
        # Cluster Groups
        # Cluster Groups Command AddGroup
        self._chipLib.chip_ime_AppendCommand_Groups_AddGroup.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_char_p]
        self._chipLib.chip_ime_AppendCommand_Groups_AddGroup.restype = ctypes.c_uint32
        # Cluster Groups Command AddGroupIfIdentifying
        self._chipLib.chip_ime_AppendCommand_Groups_AddGroupIfIdentifying.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_char_p]
        self._chipLib.chip_ime_AppendCommand_Groups_AddGroupIfIdentifying.restype = ctypes.c_uint32
        # Cluster Groups Command GetGroupMembership
        self._chipLib.chip_ime_AppendCommand_Groups_GetGroupMembership.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_Groups_GetGroupMembership.restype = ctypes.c_uint32
        # Cluster Groups Command RemoveAllGroups
        self._chipLib.chip_ime_AppendCommand_Groups_RemoveAllGroups.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_Groups_RemoveAllGroups.restype = ctypes.c_uint32
        # Cluster Groups Command RemoveGroup
        self._chipLib.chip_ime_AppendCommand_Groups_RemoveGroup.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_Groups_RemoveGroup.restype = ctypes.c_uint32
        # Cluster Groups Command ViewGroup
        self._chipLib.chip_ime_AppendCommand_Groups_ViewGroup.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_Groups_ViewGroup.restype = ctypes.c_uint32
        # Cluster IasZone
        # Cluster Identify
        # Cluster Identify Command Identify
        self._chipLib.chip_ime_AppendCommand_Identify_Identify.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_Identify_Identify.restype = ctypes.c_uint32
        # Cluster Identify Command IdentifyQuery
        self._chipLib.chip_ime_AppendCommand_Identify_IdentifyQuery.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_Identify_IdentifyQuery.restype = ctypes.c_uint32
        # Cluster LevelControl
        # Cluster LevelControl Command Move
        self._chipLib.chip_ime_AppendCommand_LevelControl_Move.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_LevelControl_Move.restype = ctypes.c_uint32
        # Cluster LevelControl Command MoveToLevel
        self._chipLib.chip_ime_AppendCommand_LevelControl_MoveToLevel.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_LevelControl_MoveToLevel.restype = ctypes.c_uint32
        # Cluster LevelControl Command MoveToLevelWithOnOff
        self._chipLib.chip_ime_AppendCommand_LevelControl_MoveToLevelWithOnOff.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_LevelControl_MoveToLevelWithOnOff.restype = ctypes.c_uint32
        # Cluster LevelControl Command MoveWithOnOff
        self._chipLib.chip_ime_AppendCommand_LevelControl_MoveWithOnOff.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_LevelControl_MoveWithOnOff.restype = ctypes.c_uint32
        # Cluster LevelControl Command Step
        self._chipLib.chip_ime_AppendCommand_LevelControl_Step.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_LevelControl_Step.restype = ctypes.c_uint32
        # Cluster LevelControl Command StepWithOnOff
        self._chipLib.chip_ime_AppendCommand_LevelControl_StepWithOnOff.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_LevelControl_StepWithOnOff.restype = ctypes.c_uint32
        # Cluster LevelControl Command Stop
        self._chipLib.chip_ime_AppendCommand_LevelControl_Stop.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_LevelControl_Stop.restype = ctypes.c_uint32
        # Cluster LevelControl Command StopWithOnOff
        self._chipLib.chip_ime_AppendCommand_LevelControl_StopWithOnOff.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_LevelControl_StopWithOnOff.restype = ctypes.c_uint32
        # Cluster OnOff
        # Cluster OnOff Command Off
        self._chipLib.chip_ime_AppendCommand_OnOff_Off.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_OnOff_Off.restype = ctypes.c_uint32
        # Cluster OnOff Command On
        self._chipLib.chip_ime_AppendCommand_OnOff_On.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_OnOff_On.restype = ctypes.c_uint32
        # Cluster OnOff Command Toggle
        self._chipLib.chip_ime_AppendCommand_OnOff_Toggle.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_OnOff_Toggle.restype = ctypes.c_uint32
        # Cluster Scenes
        # Cluster Scenes Command AddScene
        self._chipLib.chip_ime_AppendCommand_Scenes_AddScene.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_char_p, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_Scenes_AddScene.restype = ctypes.c_uint32
        # Cluster Scenes Command GetSceneMembership
        self._chipLib.chip_ime_AppendCommand_Scenes_GetSceneMembership.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_Scenes_GetSceneMembership.restype = ctypes.c_uint32
        # Cluster Scenes Command RecallScene
        self._chipLib.chip_ime_AppendCommand_Scenes_RecallScene.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_Scenes_RecallScene.restype = ctypes.c_uint32
        # Cluster Scenes Command RemoveAllScenes
        self._chipLib.chip_ime_AppendCommand_Scenes_RemoveAllScenes.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16]
        self._chipLib.chip_ime_AppendCommand_Scenes_RemoveAllScenes.restype = ctypes.c_uint32
        # Cluster Scenes Command RemoveScene
        self._chipLib.chip_ime_AppendCommand_Scenes_RemoveScene.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_Scenes_RemoveScene.restype = ctypes.c_uint32
        # Cluster Scenes Command StoreScene
        self._chipLib.chip_ime_AppendCommand_Scenes_StoreScene.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_Scenes_StoreScene.restype = ctypes.c_uint32
        # Cluster Scenes Command ViewScene
        self._chipLib.chip_ime_AppendCommand_Scenes_ViewScene.argtypes = [ctypes.c_void_p, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8]
        self._chipLib.chip_ime_AppendCommand_Scenes_ViewScene.restype = ctypes.c_uint32
        # Cluster TemperatureMeasurement
