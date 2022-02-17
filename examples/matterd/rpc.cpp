/*
 *
 *    Copyright (c) 2021 Project CHIP Authors
 *    All rights reserved.
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

#include "pw_rpc/server.h"
#include "pw_rpc_system_server/rpc_server.h"
#include "rpc/matter_controller.h"
#include <rpc.h>

#include <thread>

namespace chip {
namespace rpc {

RPCServer RPCServer::mInstance;

namespace {

chip::rpc::MatterController sMatterController;

void RegisterServices(pw::rpc::Server & server)
{
    server.RegisterService(sMatterController);
}

} // namespace

void RunRpcService()
{
    pw::rpc::system_server::Init();
    RegisterServices(pw::rpc::system_server::Server());
    pw::rpc::system_server::Start();
}

int Init()
{
    int err = 0;
    std::thread rpc_service(RunRpcService);
    rpc_service.detach();
    return err;
}

} // namespace rpc
} // namespace chip
