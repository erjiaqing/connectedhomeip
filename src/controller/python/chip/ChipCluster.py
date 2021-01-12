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
    def __init__(self, chipstack, devctl):
        self._ChipStack = chipstack
        self._devCtrl = devctl

    def SendCommand(self, nodeId: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_SendCommand(nodeId)
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

    def PrepareCommand(self, cluster: str, command: str, endpoint: int, groupid: int, args):
        func = getattr(self, "Cluster{}_Command{}".format(cluster, command), None)
        if not func:
            raise UnknownCommand(cluster, command)
        func(endpoint, groupid, **args)

    def ClusterBarrierControl_CommandBarrierControlGoToPercent(self, ZCLendpoint: int, ZCLgroupid: int, percentOpen: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_BarrierControl_BarrierControlGoToPercent(
                ZCLendpoint, ZCLgroupid, percentOpen
            )
        )

    def ClusterBarrierControl_CommandBarrierControlStop(self, ZCLendpoint: int, ZCLgroupid: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_BarrierControl_BarrierControlStop(
                ZCLendpoint, ZCLgroupid
            )
        )

    def ClusterBasic_CommandResetToFactoryDefaults(self, ZCLendpoint: int, ZCLgroupid: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_Basic_ResetToFactoryDefaults(
                ZCLendpoint, ZCLgroupid
            )
        )

    def ClusterColorControl_CommandMoveColor(self, ZCLendpoint: int, ZCLgroupid: int, rateX: int, rateY: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_ColorControl_MoveColor(
                ZCLendpoint, ZCLgroupid, rateX, rateY, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandMoveColorTemperature(self, ZCLendpoint: int, ZCLgroupid: int, moveMode: int, rate: int, colorTemperatureMinimum: int, colorTemperatureMaximum: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_ColorControl_MoveColorTemperature(
                ZCLendpoint, ZCLgroupid, moveMode, rate, colorTemperatureMinimum, colorTemperatureMaximum, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandMoveHue(self, ZCLendpoint: int, ZCLgroupid: int, moveMode: int, rate: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_ColorControl_MoveHue(
                ZCLendpoint, ZCLgroupid, moveMode, rate, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandMoveSaturation(self, ZCLendpoint: int, ZCLgroupid: int, moveMode: int, rate: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_ColorControl_MoveSaturation(
                ZCLendpoint, ZCLgroupid, moveMode, rate, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandMoveToColor(self, ZCLendpoint: int, ZCLgroupid: int, colorX: int, colorY: int, transitionTime: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_ColorControl_MoveToColor(
                ZCLendpoint, ZCLgroupid, colorX, colorY, transitionTime, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandMoveToColorTemperature(self, ZCLendpoint: int, ZCLgroupid: int, colorTemperature: int, transitionTime: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_ColorControl_MoveToColorTemperature(
                ZCLendpoint, ZCLgroupid, colorTemperature, transitionTime, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandMoveToHue(self, ZCLendpoint: int, ZCLgroupid: int, hue: int, direction: int, transitionTime: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_ColorControl_MoveToHue(
                ZCLendpoint, ZCLgroupid, hue, direction, transitionTime, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandMoveToHueAndSaturation(self, ZCLendpoint: int, ZCLgroupid: int, hue: int, saturation: int, transitionTime: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_ColorControl_MoveToHueAndSaturation(
                ZCLendpoint, ZCLgroupid, hue, saturation, transitionTime, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandMoveToSaturation(self, ZCLendpoint: int, ZCLgroupid: int, saturation: int, transitionTime: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_ColorControl_MoveToSaturation(
                ZCLendpoint, ZCLgroupid, saturation, transitionTime, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandStepColor(self, ZCLendpoint: int, ZCLgroupid: int, stepX: int, stepY: int, transitionTime: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_ColorControl_StepColor(
                ZCLendpoint, ZCLgroupid, stepX, stepY, transitionTime, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandStepColorTemperature(self, ZCLendpoint: int, ZCLgroupid: int, stepMode: int, stepSize: int, transitionTime: int, colorTemperatureMinimum: int, colorTemperatureMaximum: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_ColorControl_StepColorTemperature(
                ZCLendpoint, ZCLgroupid, stepMode, stepSize, transitionTime, colorTemperatureMinimum, colorTemperatureMaximum, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandStepHue(self, ZCLendpoint: int, ZCLgroupid: int, stepMode: int, stepSize: int, transitionTime: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_ColorControl_StepHue(
                ZCLendpoint, ZCLgroupid, stepMode, stepSize, transitionTime, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandStepSaturation(self, ZCLendpoint: int, ZCLgroupid: int, stepMode: int, stepSize: int, transitionTime: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_ColorControl_StepSaturation(
                ZCLendpoint, ZCLgroupid, stepMode, stepSize, transitionTime, optionsMask, optionsOverride
            )
        )

    def ClusterColorControl_CommandStopMoveStep(self, ZCLendpoint: int, ZCLgroupid: int, optionsMask: int, optionsOverride: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_ColorControl_StopMoveStep(
                ZCLendpoint, ZCLgroupid, optionsMask, optionsOverride
            )
        )

    def ClusterDoorLock_CommandClearAllPins(self, ZCLendpoint: int, ZCLgroupid: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_ClearAllPins(
                ZCLendpoint, ZCLgroupid
            )
        )

    def ClusterDoorLock_CommandClearAllRfids(self, ZCLendpoint: int, ZCLgroupid: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_ClearAllRfids(
                ZCLendpoint, ZCLgroupid
            )
        )

    def ClusterDoorLock_CommandClearHolidaySchedule(self, ZCLendpoint: int, ZCLgroupid: int, scheduleId: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_ClearHolidaySchedule(
                ZCLendpoint, ZCLgroupid, scheduleId
            )
        )

    def ClusterDoorLock_CommandClearPin(self, ZCLendpoint: int, ZCLgroupid: int, userId: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_ClearPin(
                ZCLendpoint, ZCLgroupid, userId
            )
        )

    def ClusterDoorLock_CommandClearRfid(self, ZCLendpoint: int, ZCLgroupid: int, userId: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_ClearRfid(
                ZCLendpoint, ZCLgroupid, userId
            )
        )

    def ClusterDoorLock_CommandClearWeekdaySchedule(self, ZCLendpoint: int, ZCLgroupid: int, scheduleId: int, userId: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_ClearWeekdaySchedule(
                ZCLendpoint, ZCLgroupid, scheduleId, userId
            )
        )

    def ClusterDoorLock_CommandClearYeardaySchedule(self, ZCLendpoint: int, ZCLgroupid: int, scheduleId: int, userId: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_ClearYeardaySchedule(
                ZCLendpoint, ZCLgroupid, scheduleId, userId
            )
        )

    def ClusterDoorLock_CommandGetHolidaySchedule(self, ZCLendpoint: int, ZCLgroupid: int, scheduleId: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_GetHolidaySchedule(
                ZCLendpoint, ZCLgroupid, scheduleId
            )
        )

    def ClusterDoorLock_CommandGetLogRecord(self, ZCLendpoint: int, ZCLgroupid: int, logIndex: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_GetLogRecord(
                ZCLendpoint, ZCLgroupid, logIndex
            )
        )

    def ClusterDoorLock_CommandGetPin(self, ZCLendpoint: int, ZCLgroupid: int, userId: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_GetPin(
                ZCLendpoint, ZCLgroupid, userId
            )
        )

    def ClusterDoorLock_CommandGetRfid(self, ZCLendpoint: int, ZCLgroupid: int, userId: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_GetRfid(
                ZCLendpoint, ZCLgroupid, userId
            )
        )

    def ClusterDoorLock_CommandGetUserType(self, ZCLendpoint: int, ZCLgroupid: int, userId: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_GetUserType(
                ZCLendpoint, ZCLgroupid, userId
            )
        )

    def ClusterDoorLock_CommandGetWeekdaySchedule(self, ZCLendpoint: int, ZCLgroupid: int, scheduleId: int, userId: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_GetWeekdaySchedule(
                ZCLendpoint, ZCLgroupid, scheduleId, userId
            )
        )

    def ClusterDoorLock_CommandGetYeardaySchedule(self, ZCLendpoint: int, ZCLgroupid: int, scheduleId: int, userId: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_GetYeardaySchedule(
                ZCLendpoint, ZCLgroupid, scheduleId, userId
            )
        )

    def ClusterDoorLock_CommandLockDoor(self, ZCLendpoint: int, ZCLgroupid: int, pin: str):
        pin = pin.encode("utf-8") + b'\x00'
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_LockDoor(
                ZCLendpoint, ZCLgroupid, pin
            )
        )

    def ClusterDoorLock_CommandSetHolidaySchedule(self, ZCLendpoint: int, ZCLgroupid: int, scheduleId: int, localStartTime: int, localEndTime: int, operatingModeDuringHoliday: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_SetHolidaySchedule(
                ZCLendpoint, ZCLgroupid, scheduleId, localStartTime, localEndTime, operatingModeDuringHoliday
            )
        )

    def ClusterDoorLock_CommandSetPin(self, ZCLendpoint: int, ZCLgroupid: int, userId: int, userStatus: int, userType: int, pin: str):
        pin = pin.encode("utf-8") + b'\x00'
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_SetPin(
                ZCLendpoint, ZCLgroupid, userId, userStatus, userType, pin
            )
        )

    def ClusterDoorLock_CommandSetRfid(self, ZCLendpoint: int, ZCLgroupid: int, userId: int, userStatus: int, userType: int, id: str):
        id = id.encode("utf-8") + b'\x00'
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_SetRfid(
                ZCLendpoint, ZCLgroupid, userId, userStatus, userType, id
            )
        )

    def ClusterDoorLock_CommandSetUserType(self, ZCLendpoint: int, ZCLgroupid: int, userId: int, userType: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_SetUserType(
                ZCLendpoint, ZCLgroupid, userId, userType
            )
        )

    def ClusterDoorLock_CommandSetWeekdaySchedule(self, ZCLendpoint: int, ZCLgroupid: int, scheduleId: int, userId: int, daysMask: int, startHour: int, startMinute: int, endHour: int, endMinute: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_SetWeekdaySchedule(
                ZCLendpoint, ZCLgroupid, scheduleId, userId, daysMask, startHour, startMinute, endHour, endMinute
            )
        )

    def ClusterDoorLock_CommandSetYeardaySchedule(self, ZCLendpoint: int, ZCLgroupid: int, scheduleId: int, userId: int, localStartTime: int, localEndTime: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_SetYeardaySchedule(
                ZCLendpoint, ZCLgroupid, scheduleId, userId, localStartTime, localEndTime
            )
        )

    def ClusterDoorLock_CommandUnlockDoor(self, ZCLendpoint: int, ZCLgroupid: int, pin: str):
        pin = pin.encode("utf-8") + b'\x00'
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_UnlockDoor(
                ZCLendpoint, ZCLgroupid, pin
            )
        )

    def ClusterDoorLock_CommandUnlockWithTimeout(self, ZCLendpoint: int, ZCLgroupid: int, timeoutInSeconds: int, pin: str):
        pin = pin.encode("utf-8") + b'\x00'
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_DoorLock_UnlockWithTimeout(
                ZCLendpoint, ZCLgroupid, timeoutInSeconds, pin
            )
        )

    def ClusterGroups_CommandAddGroup(self, ZCLendpoint: int, ZCLgroupid: int, groupId: int, groupName: str):
        groupName = groupName.encode("utf-8") + b'\x00'
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_Groups_AddGroup(
                ZCLendpoint, ZCLgroupid, groupId, groupName
            )
        )

    def ClusterGroups_CommandAddGroupIfIdentifying(self, ZCLendpoint: int, ZCLgroupid: int, groupId: int, groupName: str):
        groupName = groupName.encode("utf-8") + b'\x00'
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_Groups_AddGroupIfIdentifying(
                ZCLendpoint, ZCLgroupid, groupId, groupName
            )
        )

    def ClusterGroups_CommandGetGroupMembership(self, ZCLendpoint: int, ZCLgroupid: int, groupCount: int, groupList: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_Groups_GetGroupMembership(
                ZCLendpoint, ZCLgroupid, groupCount, groupList
            )
        )

    def ClusterGroups_CommandRemoveAllGroups(self, ZCLendpoint: int, ZCLgroupid: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_Groups_RemoveAllGroups(
                ZCLendpoint, ZCLgroupid
            )
        )

    def ClusterGroups_CommandRemoveGroup(self, ZCLendpoint: int, ZCLgroupid: int, groupId: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_Groups_RemoveGroup(
                ZCLendpoint, ZCLgroupid, groupId
            )
        )

    def ClusterGroups_CommandViewGroup(self, ZCLendpoint: int, ZCLgroupid: int, groupId: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_Groups_ViewGroup(
                ZCLendpoint, ZCLgroupid, groupId
            )
        )

    def ClusterIdentify_CommandIdentify(self, ZCLendpoint: int, ZCLgroupid: int, identifyTime: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_Identify_Identify(
                ZCLendpoint, ZCLgroupid, identifyTime
            )
        )

    def ClusterIdentify_CommandIdentifyQuery(self, ZCLendpoint: int, ZCLgroupid: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_Identify_IdentifyQuery(
                ZCLendpoint, ZCLgroupid
            )
        )

    def ClusterLevelControl_CommandMove(self, ZCLendpoint: int, ZCLgroupid: int, moveMode: int, rate: int, optionMask: int, optionOverride: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_LevelControl_Move(
                ZCLendpoint, ZCLgroupid, moveMode, rate, optionMask, optionOverride
            )
        )

    def ClusterLevelControl_CommandMoveToLevel(self, ZCLendpoint: int, ZCLgroupid: int, level: int, transitionTime: int, optionMask: int, optionOverride: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_LevelControl_MoveToLevel(
                ZCLendpoint, ZCLgroupid, level, transitionTime, optionMask, optionOverride
            )
        )

    def ClusterLevelControl_CommandMoveToLevelWithOnOff(self, ZCLendpoint: int, ZCLgroupid: int, level: int, transitionTime: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_LevelControl_MoveToLevelWithOnOff(
                ZCLendpoint, ZCLgroupid, level, transitionTime
            )
        )

    def ClusterLevelControl_CommandMoveWithOnOff(self, ZCLendpoint: int, ZCLgroupid: int, moveMode: int, rate: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_LevelControl_MoveWithOnOff(
                ZCLendpoint, ZCLgroupid, moveMode, rate
            )
        )

    def ClusterLevelControl_CommandStep(self, ZCLendpoint: int, ZCLgroupid: int, stepMode: int, stepSize: int, transitionTime: int, optionMask: int, optionOverride: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_LevelControl_Step(
                ZCLendpoint, ZCLgroupid, stepMode, stepSize, transitionTime, optionMask, optionOverride
            )
        )

    def ClusterLevelControl_CommandStepWithOnOff(self, ZCLendpoint: int, ZCLgroupid: int, stepMode: int, stepSize: int, transitionTime: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_LevelControl_StepWithOnOff(
                ZCLendpoint, ZCLgroupid, stepMode, stepSize, transitionTime
            )
        )

    def ClusterLevelControl_CommandStop(self, ZCLendpoint: int, ZCLgroupid: int, optionMask: int, optionOverride: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_LevelControl_Stop(
                ZCLendpoint, ZCLgroupid, optionMask, optionOverride
            )
        )

    def ClusterLevelControl_CommandStopWithOnOff(self, ZCLendpoint: int, ZCLgroupid: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_LevelControl_StopWithOnOff(
                ZCLendpoint, ZCLgroupid
            )
        )

    def ClusterOnOff_CommandOff(self, ZCLendpoint: int, ZCLgroupid: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_OnOff_Off(
                ZCLendpoint, ZCLgroupid
            )
        )

    def ClusterOnOff_CommandOn(self, ZCLendpoint: int, ZCLgroupid: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_OnOff_On(
                ZCLendpoint, ZCLgroupid
            )
        )

    def ClusterOnOff_CommandToggle(self, ZCLendpoint: int, ZCLgroupid: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_OnOff_Toggle(
                ZCLendpoint, ZCLgroupid
            )
        )

    def ClusterScenes_CommandAddScene(self, ZCLendpoint: int, ZCLgroupid: int, groupId: int, sceneId: int, transitionTime: int, sceneName: str, clusterId: int, length: int, value: int):
        sceneName = sceneName.encode("utf-8") + b'\x00'
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_Scenes_AddScene(
                ZCLendpoint, ZCLgroupid, groupId, sceneId, transitionTime, sceneName, clusterId, length, value
            )
        )

    def ClusterScenes_CommandGetSceneMembership(self, ZCLendpoint: int, ZCLgroupid: int, groupId: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_Scenes_GetSceneMembership(
                ZCLendpoint, ZCLgroupid, groupId
            )
        )

    def ClusterScenes_CommandRecallScene(self, ZCLendpoint: int, ZCLgroupid: int, groupId: int, sceneId: int, transitionTime: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_Scenes_RecallScene(
                ZCLendpoint, ZCLgroupid, groupId, sceneId, transitionTime
            )
        )

    def ClusterScenes_CommandRemoveAllScenes(self, ZCLendpoint: int, ZCLgroupid: int, groupId: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_Scenes_RemoveAllScenes(
                ZCLendpoint, ZCLgroupid, groupId
            )
        )

    def ClusterScenes_CommandRemoveScene(self, ZCLendpoint: int, ZCLgroupid: int, groupId: int, sceneId: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_Scenes_RemoveScene(
                ZCLendpoint, ZCLgroupid, groupId, sceneId
            )
        )

    def ClusterScenes_CommandStoreScene(self, ZCLendpoint: int, ZCLgroupid: int, groupId: int, sceneId: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_Scenes_StoreScene(
                ZCLendpoint, ZCLgroupid, groupId, sceneId
            )
        )

    def ClusterScenes_CommandViewScene(self, ZCLendpoint: int, ZCLgroupid: int, groupId: int, sceneId: int):
        self._ChipStack.Call(
            lambda: self._dmLib.chip_ime_AppendCommand_Scenes_ViewScene(
                ZCLendpoint, ZCLgroupid, groupId, sceneId
            )
        )

    def InitLib(self, chipLib):
        self._dmLib = chipLib
        self._dmLib.chip_ime_SendCommand.argtypes = [ctypes.c_uint64]
        self._dmLib.chip_ime_SendCommand.restype = ctypes.c_uint32
        # Cluster BarrierControl
        # Cluster BarrierControl Command BarrierControlGoToPercent
        self._dmLib.chip_ime_AppendCommand_BarrierControl_BarrierControlGoToPercent.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_BarrierControl_BarrierControlGoToPercent.restype = ctypes.c_uint32
        # Cluster BarrierControl Command BarrierControlStop
        self._dmLib.chip_ime_AppendCommand_BarrierControl_BarrierControlStop.argtypes = [ctypes.c_uint8, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_BarrierControl_BarrierControlStop.restype = ctypes.c_uint32
        # Cluster Basic
        # Cluster Basic Command ResetToFactoryDefaults
        self._dmLib.chip_ime_AppendCommand_Basic_ResetToFactoryDefaults.argtypes = [ctypes.c_uint8, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_Basic_ResetToFactoryDefaults.restype = ctypes.c_uint32
        # Cluster ColorControl
        # Cluster ColorControl Command MoveColor
        self._dmLib.chip_ime_AppendCommand_ColorControl_MoveColor.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_int16, ctypes.c_int16, ctypes.c_uint8, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_ColorControl_MoveColor.restype = ctypes.c_uint32
        # Cluster ColorControl Command MoveColorTemperature
        self._dmLib.chip_ime_AppendCommand_ColorControl_MoveColorTemperature.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_ColorControl_MoveColorTemperature.restype = ctypes.c_uint32
        # Cluster ColorControl Command MoveHue
        self._dmLib.chip_ime_AppendCommand_ColorControl_MoveHue.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_ColorControl_MoveHue.restype = ctypes.c_uint32
        # Cluster ColorControl Command MoveSaturation
        self._dmLib.chip_ime_AppendCommand_ColorControl_MoveSaturation.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_ColorControl_MoveSaturation.restype = ctypes.c_uint32
        # Cluster ColorControl Command MoveToColor
        self._dmLib.chip_ime_AppendCommand_ColorControl_MoveToColor.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_ColorControl_MoveToColor.restype = ctypes.c_uint32
        # Cluster ColorControl Command MoveToColorTemperature
        self._dmLib.chip_ime_AppendCommand_ColorControl_MoveToColorTemperature.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_ColorControl_MoveToColorTemperature.restype = ctypes.c_uint32
        # Cluster ColorControl Command MoveToHue
        self._dmLib.chip_ime_AppendCommand_ColorControl_MoveToHue.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_ColorControl_MoveToHue.restype = ctypes.c_uint32
        # Cluster ColorControl Command MoveToHueAndSaturation
        self._dmLib.chip_ime_AppendCommand_ColorControl_MoveToHueAndSaturation.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_ColorControl_MoveToHueAndSaturation.restype = ctypes.c_uint32
        # Cluster ColorControl Command MoveToSaturation
        self._dmLib.chip_ime_AppendCommand_ColorControl_MoveToSaturation.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_ColorControl_MoveToSaturation.restype = ctypes.c_uint32
        # Cluster ColorControl Command StepColor
        self._dmLib.chip_ime_AppendCommand_ColorControl_StepColor.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_int16, ctypes.c_int16, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_ColorControl_StepColor.restype = ctypes.c_uint32
        # Cluster ColorControl Command StepColorTemperature
        self._dmLib.chip_ime_AppendCommand_ColorControl_StepColorTemperature.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_ColorControl_StepColorTemperature.restype = ctypes.c_uint32
        # Cluster ColorControl Command StepHue
        self._dmLib.chip_ime_AppendCommand_ColorControl_StepHue.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_ColorControl_StepHue.restype = ctypes.c_uint32
        # Cluster ColorControl Command StepSaturation
        self._dmLib.chip_ime_AppendCommand_ColorControl_StepSaturation.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_ColorControl_StepSaturation.restype = ctypes.c_uint32
        # Cluster ColorControl Command StopMoveStep
        self._dmLib.chip_ime_AppendCommand_ColorControl_StopMoveStep.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_ColorControl_StopMoveStep.restype = ctypes.c_uint32
        # Cluster DoorLock
        # Cluster DoorLock Command ClearAllPins
        self._dmLib.chip_ime_AppendCommand_DoorLock_ClearAllPins.argtypes = [ctypes.c_uint8, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_DoorLock_ClearAllPins.restype = ctypes.c_uint32
        # Cluster DoorLock Command ClearAllRfids
        self._dmLib.chip_ime_AppendCommand_DoorLock_ClearAllRfids.argtypes = [ctypes.c_uint8, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_DoorLock_ClearAllRfids.restype = ctypes.c_uint32
        # Cluster DoorLock Command ClearHolidaySchedule
        self._dmLib.chip_ime_AppendCommand_DoorLock_ClearHolidaySchedule.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_DoorLock_ClearHolidaySchedule.restype = ctypes.c_uint32
        # Cluster DoorLock Command ClearPin
        self._dmLib.chip_ime_AppendCommand_DoorLock_ClearPin.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_DoorLock_ClearPin.restype = ctypes.c_uint32
        # Cluster DoorLock Command ClearRfid
        self._dmLib.chip_ime_AppendCommand_DoorLock_ClearRfid.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_DoorLock_ClearRfid.restype = ctypes.c_uint32
        # Cluster DoorLock Command ClearWeekdaySchedule
        self._dmLib.chip_ime_AppendCommand_DoorLock_ClearWeekdaySchedule.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_DoorLock_ClearWeekdaySchedule.restype = ctypes.c_uint32
        # Cluster DoorLock Command ClearYeardaySchedule
        self._dmLib.chip_ime_AppendCommand_DoorLock_ClearYeardaySchedule.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_DoorLock_ClearYeardaySchedule.restype = ctypes.c_uint32
        # Cluster DoorLock Command GetHolidaySchedule
        self._dmLib.chip_ime_AppendCommand_DoorLock_GetHolidaySchedule.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_DoorLock_GetHolidaySchedule.restype = ctypes.c_uint32
        # Cluster DoorLock Command GetLogRecord
        self._dmLib.chip_ime_AppendCommand_DoorLock_GetLogRecord.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_DoorLock_GetLogRecord.restype = ctypes.c_uint32
        # Cluster DoorLock Command GetPin
        self._dmLib.chip_ime_AppendCommand_DoorLock_GetPin.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_DoorLock_GetPin.restype = ctypes.c_uint32
        # Cluster DoorLock Command GetRfid
        self._dmLib.chip_ime_AppendCommand_DoorLock_GetRfid.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_DoorLock_GetRfid.restype = ctypes.c_uint32
        # Cluster DoorLock Command GetUserType
        self._dmLib.chip_ime_AppendCommand_DoorLock_GetUserType.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_DoorLock_GetUserType.restype = ctypes.c_uint32
        # Cluster DoorLock Command GetWeekdaySchedule
        self._dmLib.chip_ime_AppendCommand_DoorLock_GetWeekdaySchedule.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_DoorLock_GetWeekdaySchedule.restype = ctypes.c_uint32
        # Cluster DoorLock Command GetYeardaySchedule
        self._dmLib.chip_ime_AppendCommand_DoorLock_GetYeardaySchedule.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_DoorLock_GetYeardaySchedule.restype = ctypes.c_uint32
        # Cluster DoorLock Command LockDoor
        self._dmLib.chip_ime_AppendCommand_DoorLock_LockDoor.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_char_p]
        self._dmLib.chip_ime_AppendCommand_DoorLock_LockDoor.restype = ctypes.c_uint32
        # Cluster DoorLock Command SetHolidaySchedule
        self._dmLib.chip_ime_AppendCommand_DoorLock_SetHolidaySchedule.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint32, ctypes.c_uint32, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_DoorLock_SetHolidaySchedule.restype = ctypes.c_uint32
        # Cluster DoorLock Command SetPin
        self._dmLib.chip_ime_AppendCommand_DoorLock_SetPin.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_char_p]
        self._dmLib.chip_ime_AppendCommand_DoorLock_SetPin.restype = ctypes.c_uint32
        # Cluster DoorLock Command SetRfid
        self._dmLib.chip_ime_AppendCommand_DoorLock_SetRfid.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_char_p]
        self._dmLib.chip_ime_AppendCommand_DoorLock_SetRfid.restype = ctypes.c_uint32
        # Cluster DoorLock Command SetUserType
        self._dmLib.chip_ime_AppendCommand_DoorLock_SetUserType.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_DoorLock_SetUserType.restype = ctypes.c_uint32
        # Cluster DoorLock Command SetWeekdaySchedule
        self._dmLib.chip_ime_AppendCommand_DoorLock_SetWeekdaySchedule.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_DoorLock_SetWeekdaySchedule.restype = ctypes.c_uint32
        # Cluster DoorLock Command SetYeardaySchedule
        self._dmLib.chip_ime_AppendCommand_DoorLock_SetYeardaySchedule.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint32, ctypes.c_uint32]
        self._dmLib.chip_ime_AppendCommand_DoorLock_SetYeardaySchedule.restype = ctypes.c_uint32
        # Cluster DoorLock Command UnlockDoor
        self._dmLib.chip_ime_AppendCommand_DoorLock_UnlockDoor.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_char_p]
        self._dmLib.chip_ime_AppendCommand_DoorLock_UnlockDoor.restype = ctypes.c_uint32
        # Cluster DoorLock Command UnlockWithTimeout
        self._dmLib.chip_ime_AppendCommand_DoorLock_UnlockWithTimeout.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_char_p]
        self._dmLib.chip_ime_AppendCommand_DoorLock_UnlockWithTimeout.restype = ctypes.c_uint32
        # Cluster Groups
        # Cluster Groups Command AddGroup
        self._dmLib.chip_ime_AppendCommand_Groups_AddGroup.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_char_p]
        self._dmLib.chip_ime_AppendCommand_Groups_AddGroup.restype = ctypes.c_uint32
        # Cluster Groups Command AddGroupIfIdentifying
        self._dmLib.chip_ime_AppendCommand_Groups_AddGroupIfIdentifying.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_char_p]
        self._dmLib.chip_ime_AppendCommand_Groups_AddGroupIfIdentifying.restype = ctypes.c_uint32
        # Cluster Groups Command GetGroupMembership
        self._dmLib.chip_ime_AppendCommand_Groups_GetGroupMembership.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_Groups_GetGroupMembership.restype = ctypes.c_uint32
        # Cluster Groups Command RemoveAllGroups
        self._dmLib.chip_ime_AppendCommand_Groups_RemoveAllGroups.argtypes = [ctypes.c_uint8, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_Groups_RemoveAllGroups.restype = ctypes.c_uint32
        # Cluster Groups Command RemoveGroup
        self._dmLib.chip_ime_AppendCommand_Groups_RemoveGroup.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_Groups_RemoveGroup.restype = ctypes.c_uint32
        # Cluster Groups Command ViewGroup
        self._dmLib.chip_ime_AppendCommand_Groups_ViewGroup.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_Groups_ViewGroup.restype = ctypes.c_uint32
        # Cluster IasZone
        # Cluster Identify
        # Cluster Identify Command Identify
        self._dmLib.chip_ime_AppendCommand_Identify_Identify.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_Identify_Identify.restype = ctypes.c_uint32
        # Cluster Identify Command IdentifyQuery
        self._dmLib.chip_ime_AppendCommand_Identify_IdentifyQuery.argtypes = [ctypes.c_uint8, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_Identify_IdentifyQuery.restype = ctypes.c_uint32
        # Cluster LevelControl
        # Cluster LevelControl Command Move
        self._dmLib.chip_ime_AppendCommand_LevelControl_Move.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_LevelControl_Move.restype = ctypes.c_uint32
        # Cluster LevelControl Command MoveToLevel
        self._dmLib.chip_ime_AppendCommand_LevelControl_MoveToLevel.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_LevelControl_MoveToLevel.restype = ctypes.c_uint32
        # Cluster LevelControl Command MoveToLevelWithOnOff
        self._dmLib.chip_ime_AppendCommand_LevelControl_MoveToLevelWithOnOff.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_LevelControl_MoveToLevelWithOnOff.restype = ctypes.c_uint32
        # Cluster LevelControl Command MoveWithOnOff
        self._dmLib.chip_ime_AppendCommand_LevelControl_MoveWithOnOff.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_LevelControl_MoveWithOnOff.restype = ctypes.c_uint32
        # Cluster LevelControl Command Step
        self._dmLib.chip_ime_AppendCommand_LevelControl_Step.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_LevelControl_Step.restype = ctypes.c_uint32
        # Cluster LevelControl Command StepWithOnOff
        self._dmLib.chip_ime_AppendCommand_LevelControl_StepWithOnOff.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_LevelControl_StepWithOnOff.restype = ctypes.c_uint32
        # Cluster LevelControl Command Stop
        self._dmLib.chip_ime_AppendCommand_LevelControl_Stop.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_LevelControl_Stop.restype = ctypes.c_uint32
        # Cluster LevelControl Command StopWithOnOff
        self._dmLib.chip_ime_AppendCommand_LevelControl_StopWithOnOff.argtypes = [ctypes.c_uint8, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_LevelControl_StopWithOnOff.restype = ctypes.c_uint32
        # Cluster OnOff
        # Cluster OnOff Command Off
        self._dmLib.chip_ime_AppendCommand_OnOff_Off.argtypes = [ctypes.c_uint8, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_OnOff_Off.restype = ctypes.c_uint32
        # Cluster OnOff Command On
        self._dmLib.chip_ime_AppendCommand_OnOff_On.argtypes = [ctypes.c_uint8, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_OnOff_On.restype = ctypes.c_uint32
        # Cluster OnOff Command Toggle
        self._dmLib.chip_ime_AppendCommand_OnOff_Toggle.argtypes = [ctypes.c_uint8, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_OnOff_Toggle.restype = ctypes.c_uint32
        # Cluster Scenes
        # Cluster Scenes Command AddScene
        self._dmLib.chip_ime_AppendCommand_Scenes_AddScene.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16, ctypes.c_char_p, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_Scenes_AddScene.restype = ctypes.c_uint32
        # Cluster Scenes Command GetSceneMembership
        self._dmLib.chip_ime_AppendCommand_Scenes_GetSceneMembership.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_Scenes_GetSceneMembership.restype = ctypes.c_uint32
        # Cluster Scenes Command RecallScene
        self._dmLib.chip_ime_AppendCommand_Scenes_RecallScene.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_Scenes_RecallScene.restype = ctypes.c_uint32
        # Cluster Scenes Command RemoveAllScenes
        self._dmLib.chip_ime_AppendCommand_Scenes_RemoveAllScenes.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16]
        self._dmLib.chip_ime_AppendCommand_Scenes_RemoveAllScenes.restype = ctypes.c_uint32
        # Cluster Scenes Command RemoveScene
        self._dmLib.chip_ime_AppendCommand_Scenes_RemoveScene.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_Scenes_RemoveScene.restype = ctypes.c_uint32
        # Cluster Scenes Command StoreScene
        self._dmLib.chip_ime_AppendCommand_Scenes_StoreScene.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_Scenes_StoreScene.restype = ctypes.c_uint32
        # Cluster Scenes Command ViewScene
        self._dmLib.chip_ime_AppendCommand_Scenes_ViewScene.argtypes = [ctypes.c_uint8, ctypes.c_uint16, ctypes.c_uint16, ctypes.c_uint8]
        self._dmLib.chip_ime_AppendCommand_Scenes_ViewScene.restype = ctypes.c_uint32
        # Cluster TemperatureMeasurement
