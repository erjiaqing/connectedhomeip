/*
 *   Copyright (c) 2020 Project CHIP Authors
 *   All rights reserved.
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 */

#include "commands/common/Commands.h"

#include "commands/clusters/Commands.h"
#include "commands/pairing/Commands.h"
#include "commands/payload/Commands.h"

#include <transport/PASESession.h>

// ================================================================================
// Main Code
// ================================================================================

namespace chip {
namespace app {

void DispatchSingleClusterCommand(chip::ClusterId aClusterId, chip::CommandId aCommandId, chip::EndpointId aEndPointId,
                                  chip::TLV::TLVReader & aReader, Command * apCommandObj)
{
    ChipLogDetail(DataManagement, "Received Cluster Command: Cluster=%" PRIx16 " Command=%" PRIx8 " Endpoint=%" PRIx8, aClusterId,
                  aCommandId, aEndPointId);
    ChipLogError(
        DataManagement,
        "Default DispatchSingleClusterCommand is called, this should be replaced by actual dispatched for cluster commands");
}

} // namespace app
} // namespace chip

int main(int argc, char * argv[])
{
    Commands commands;
    registerCommandsPayload(commands);
    registerCommandsPairing(commands);
    registerClusters(commands);

    return commands.Run(chip::kTestControllerNodeId, chip::kTestDeviceNodeId, argc, argv);
}
