import ctypes
from dataclasses import dataclass
from chip.ChipStack import *
from chip.exceptions import *
import typing
from . import ClusterObjects


@dataclass
class WiFiInterfaceScanResult(ClusterObjects.ClusterObject):
    Descriptor: typing.ClassVar[ClusterObjects.ClusterObjectDescriptor] = ClusterObjects.ClusterObjectDescriptor(
        Fields=[
            ClusterObjects.ClusterObjectFieldDescriptor("security", 0, int),
            ClusterObjects.ClusterObjectFieldDescriptor("ssid", 1, bytes),
            ClusterObjects.ClusterObjectFieldDescriptor("bssid", 2, bytes),
            ClusterObjects.ClusterObjectFieldDescriptor("channel", 3, int),
            ClusterObjects.ClusterObjectFieldDescriptor(
                "frequencyBand", 4, int),
        ]
    )
    security: int
    ssid: bytes
    bssid: bytes
    channel: int
    frequencyBand: int


@dataclass
class ThreadInterfaceScanResult(ClusterObjects.ClusterObject):
    Descriptor: typing.ClassVar[ClusterObjects.ClusterObjectDescriptor] = ClusterObjects.ClusterObjectDescriptor(
        Fields=[
            ClusterObjects.ClusterObjectFieldDescriptor("resp", 0, bytes),
        ]
    )
    resp: bytes


@dataclass
class NetworkCommissioningCluster:
    ClusterId: typing.ClassVar[int] = 0x31

    class _NetworkCommissioningClusterCommand(ClusterObjects.ClusterCommand):
        @classmethod
        @property
        def ClusterId(self) -> int:
            return 0x31

    @dataclass
    class ScanNetworkCommandParams(_NetworkCommissioningClusterCommand):
        Descriptor: typing.ClassVar[ClusterObjects.ClusterObjectDescriptor] = ClusterObjects.ClusterObjectDescriptor(
            Fields=[
                ClusterObjects.ClusterObjectFieldDescriptor("ssid", 0, bytes),
                ClusterObjects.ClusterObjectFieldDescriptor(
                    "breadcrumb", 1, int),
                ClusterObjects.ClusterObjectFieldDescriptor(
                    "timeoutMs", 2, int),
            ]
        )

        @classmethod
        @property
        def CommandId(self) -> int:
            return 0

        @classmethod
        @property
        def ResponseType(self) -> typing.Union[ClusterObjects.ClusterCommand, None]:
            return NetworkCommissioningCluster.ScanNetworkResponseCommandParams

        ssid: bytes
        breadcrumb: int
        timeoutMs: int

    @dataclass
    class ScanNetworkResponseCommandParams(_NetworkCommissioningClusterCommand):
        Descriptor: typing.ClassVar[ClusterObjects.ClusterObjectDescriptor] = ClusterObjects.ClusterObjectDescriptor(
            Fields=[
                ClusterObjects.ClusterObjectFieldDescriptor(
                    "errorCode", 0, int),
                ClusterObjects.ClusterObjectFieldDescriptor(
                    "debugText", 1, str),
                ClusterObjects.ClusterObjectFieldDescriptor(
                    "wifiScanResults", 2, WiFiInterfaceScanResult, IsArray=True),
                ClusterObjects.ClusterObjectFieldDescriptor(
                    "threadScanResults", 3, ThreadInterfaceScanResult, IsArray=True)
            ]
        )

        @classmethod
        @property
        def CommandId(self) -> int:
            return 1

        errorCode: int
        debugText: str
        wifiScanResults: typing.List[WiFiInterfaceScanResult]
        threadScanResults: typing.List[ThreadInterfaceScanResult]
