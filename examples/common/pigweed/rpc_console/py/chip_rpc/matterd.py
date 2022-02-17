import server_pb2
import chip.clusters
import dataclasses
import typing


class MatterControllerHelper:
    def __init__(self, rpc):
        self._rpc = rpc

    def IPCommission(self, IP: str, Pincode: int, NodeID: int):
        return self._rpc.chip.rpc.MatterController.IPCommission(FabricIndex=1, IP=IP, Pincode=Pincode, NodeId=NodeID)

    @dataclasses.dataclass
    class InvokeCommandResponse:
        Status: int = 0
        Payload: typing.Union[None, chip.clusters.ClusterCommand] = None

    def InvokeCommand(self, NodeID: int, EndpointID: int, Payload: chip.clusters.ClusterCommand):
        commandPath = server_pb2.CommandPath(
            EndpointID=EndpointID, ClusterID=Payload.cluster_id, CommandID=Payload.command_id)
        res = self._rpc.chip.rpc.MatterController.InvokeCommand(
            FabricId=0, NodeId=NodeID, path=commandPath, payload=Payload.ToTLV())
        ret = MatterControllerHelper.InvokeCommandResponse()
        if len(res) == 2:
            ret.Status = res[1].statusCode
            if res[1].payload:
                ret.Payload = chip.clusters.Command.FindCommandClusterObject(isClientSideCommand=False, path=chip.clusters.Command.CommandPath(
                    EndpointId=res[1].path.EndpointID, ClusterId=res[1].path.ClusterID, CommandId=res[1].path.CommandID)).FromTLV(res[1].payload)
            return ret
        return res
