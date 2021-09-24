from chip.clusters import Command as clusterCommand
import asyncio
from chip import ChipDeviceCtrl
from chip.clusters.CHIPClustersTest import NetworkCommissioningCluster

devCtrl = ChipDeviceCtrl.ChipDeviceController(
    controllerNodeId=112233)

devCtrl.ConnectIP(b'127.0.0.1', 20202021, 1)

# Temporary API for setting backend device controller for sending commands, should be removed later.
clusterCommand.SetDeviceController(devCtrl)


async def runner():
    req = NetworkCommissioningCluster.ScanNetworkCommandParams(
        ssid=b'asd', breadcrumb=123, timeoutMs=1000)
    print(await req.Send(nodeId=1, endpointId=0))


asyncio.run(runner())
