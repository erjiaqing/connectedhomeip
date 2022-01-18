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

#include "app-common/zap-generated/ids/Attributes.h"
#include "app-common/zap-generated/ids/Clusters.h"
#include "app/ConcreteAttributePath.h"
#include "protocols/interaction_model/Constants.h"
#include <app-common/zap-generated/cluster-objects.h>
#include <app/AppBuildConfig.h>
#include <app/AttributeAccessInterface.h>
#include <app/CommandHandlerInterface.h>
#include <app/InteractionModelEngine.h>
#include <app/WriteClient.h>
#include <app/data-model/Decode.h>
#include <app/tests/AppTestContext.h>
#include <app/util/DataModelHandler.h>
#include <app/util/attribute-storage.h>
#include <controller/InvokeInteraction.h>
#include <lib/support/ErrorStr.h>
#include <lib/support/UnitTestRegistration.h>
#include <lib/support/logging/CHIPLogging.h>
#include <messaging/tests/MessagingContext.h>
#include <nlunit-test.h>

using TestContext = chip::Test::AppContext;
using namespace chip;
using namespace chip::app::Clusters;

namespace {

uint32_t gIterationCount = 0;
nlTestSuite * gSuite     = nullptr;

//
// The generated endpoint_config for the controller app has Endpoint 1
// already used in the fixed endpoint set of size 1. Consequently, let's use the next
// number higher than that for our dynamic test endpoint.
//
constexpr EndpointId kTestEndpointId     = 2;
constexpr AttributeId kTestListAttribute = 6;
constexpr uint32_t kTestListLength       = 5;

// We don't really care about the content, we just need a buffer.
uint8_t sByteSpanData[app::kMaxSecureSduLengthBytes];

class TestCommandInteraction
{
public:
    TestCommandInteraction() {}
    static void TestListChunking(nlTestSuite * apSuite, void * apContext);
    static void TestBadChunking(nlTestSuite * apSuite, void * apContext);

private:
};

//clang-format off
DECLARE_DYNAMIC_ATTRIBUTE_LIST_BEGIN(testClusterAttrsOnEndpoint)
DECLARE_DYNAMIC_ATTRIBUTE(kTestListAttribute, ARRAY, 1, 0), DECLARE_DYNAMIC_ATTRIBUTE_LIST_END();

DECLARE_DYNAMIC_CLUSTER_LIST_BEGIN(testEndpointClusters)
DECLARE_DYNAMIC_CLUSTER(TestCluster::Id, testClusterAttrsOnEndpoint), DECLARE_DYNAMIC_CLUSTER_LIST_END;

DECLARE_DYNAMIC_ENDPOINT(testEndpoint, testEndpointClusters);

//clang-format on

class TestWriteCallback : public app::WriteClient::Callback
{
public:
    void OnResponse(const app::WriteClient * apWriteClient, const app::ConcreteAttributePath & aPath, app::StatusIB status) override
    {
        if (status.mStatus == Protocols::InteractionModel::Status::Success)
        {
            mSuccessCount++;
        }
        else
        {
            mErrorCount++;
        }
    }

    void OnError(const app::WriteClient * apWriteClient, const app::StatusIB & aStatus, CHIP_ERROR aError) override
    {
        mErrorCount++;
    }

    void OnDone(app::WriteClient * apWriteClient) override { mOnDoneCount++; }

    uint32_t mSuccessCount = 0;
    uint32_t mErrorCount   = 0;
    uint32_t mOnDoneCount  = 0;
};

class TestAttrAccess : public app::AttributeAccessInterface
{
public:
    // Register for the Test Cluster cluster on all endpoints.
    TestAttrAccess() : AttributeAccessInterface(Optional<EndpointId>::Missing(), TestCluster::Id) {}

    CHIP_ERROR Read(const app::ConcreteReadAttributePath & aPath, app::AttributeValueEncoder & aEncoder) override;
    CHIP_ERROR Write(const app::ConcreteDataAttributePath & aPath, app::AttributeValueDecoder & aDecoder) override;
};

CHIP_ERROR TestAttrAccess::Read(const app::ConcreteReadAttributePath & aPath, app::AttributeValueEncoder & aEncoder)
{
    return CHIP_ERROR_UNSUPPORTED_CHIP_FEATURE;
}

CHIP_ERROR TestAttrAccess::Write(const app::ConcreteDataAttributePath & aPath, app::AttributeValueDecoder & aDecoder)
{
    // We only care about the number of attribute data.
    return CHIP_NO_ERROR;
}

/*
 * This validates all the various corner cases encountered during chunking by
 * artificially reducing the size of a packet buffer used to encode attribute data
 * to force chunking to happen over multiple packets even with a small number of attributes
 * and then slowly increasing the available size by 1 byte in each test iteration and re-running
 * the report generation logic. This 1-byte incremental approach sweeps through from a base scenario of
 * N attributes fitting in a report, to eventually resulting in N+1 attributes fitting in a report.

 * This will cause all the various corner cases encountered of closing out the various containers within
 * the report and thoroughly and definitely validate those edge cases.
 *
 * Importantly, this test tries to re-use *as much as possible* the actual IM constructs used by real
 * server-side applications. Consequently, this is why it registers a dynamic endpoint + fake attribute access
 * interface to simulate faithfully a real application. This ensures validation of as much production logic pathways
 * as we can possibly cover.
 *
 */
void TestCommandInteraction::TestListChunking(nlTestSuite * apSuite, void * apContext)
{
    TestContext & ctx  = *static_cast<TestContext *>(apContext);
    auto sessionHandle = ctx.GetSessionBobToAlice();
    TestAttrAccess testServer;

    // Initialize the ember side server logic
    InitDataModelHandler(&ctx.GetExchangeManager());

    // Register our fake dynamic endpoint.
    emberAfSetDynamicEndpoint(0, kTestEndpointId, &testEndpoint, 0, 0);

    // Register our fake attribute access interface.
    registerAttributeAccessOverride(&testServer);

    app::AttributePathParams attributePath(kTestEndpointId, app::Clusters::TestCluster::Id, kTestListAttribute);
    //
    // We've empirically determined that by reserving 950 bytes in the packet buffer, we can fit 2
    // AttributeDataIBs into the packet. ~30-40 bytes covers a single write chunk, but let's 2-3x that
    // to ensure we'll sweep from fitting 2 chunks to 3-4 chunks.
    //
    for (int i = 100; i > 0; i--)
    {
        TestWriteCallback writeCallback;

        ChipLogDetail(DataManagement, "Running iteration %d\n", i);

        gIterationCount = (uint32_t) i;

        app::WriteClient writeClient(&ctx.GetExchangeManager(), &writeCallback, Optional<uint16_t>::Missing());
        writeClient.SetWriterReserved(static_cast<uint32_t>(850 + i));

        uint8_t list[kTestListLength];
        for (uint8_t j = 0; j < kTestListLength; j++)
        {
            list[j] = static_cast<uint8_t>(i);
        }

        writeClient.EncodeAttributeWritePayload(attributePath, app::DataModel::List<uint8_t>(list, kTestListLength));
        writeClient.SendWriteRequest(sessionHandle);

        //
        // Service the IO + Engine till we get a ReportEnd callback on the client.
        // Since bugs can happen, we don't want this test to never stop, so create a ceiling for how many
        // times this can run without seeing expected results.
        //
        for (int j = 0; j < 10 && writeCallback.mOnDoneCount == 0; j++)
        {
            ctx.DrainAndServiceIO();
        }

        NL_TEST_ASSERT(apSuite, writeCallback.mSuccessCount == kTestListLength);
        NL_TEST_ASSERT(apSuite, writeCallback.mErrorCount == 0);
        NL_TEST_ASSERT(apSuite, writeCallback.mOnDoneCount == 1);

        NL_TEST_ASSERT(apSuite, ctx.GetExchangeManager().GetNumActiveExchanges() == 0);

        //
        // Stop the test if we detected an error. Otherwise, it'll be difficult to read the logs.
        //
        if (apSuite->flagError)
        {
            break;
        }
    }
}

// Read an attribute that can never fit into the buffer. Result in an empty report, client should reject the request.
void TestCommandInteraction::TestBadChunking(nlTestSuite * apSuite, void * apContext)
{
    TestContext & ctx  = *static_cast<TestContext *>(apContext);
    auto sessionHandle = ctx.GetSessionBobToAlice();
    TestAttrAccess testServer;

    bool atLeastOneRequestSent   = false;
    bool atLeastOneRequestFailed = false;

    // Initialize the ember side server logic
    InitDataModelHandler(&ctx.GetExchangeManager());

    // Register our fake dynamic endpoint.
    emberAfSetDynamicEndpoint(0, kTestEndpointId, &testEndpoint, 0, 0);

    // Register our fake attribute access interface.
    registerAttributeAccessOverride(&testServer);

    app::AttributePathParams attributePath(kTestEndpointId, app::Clusters::TestCluster::Id, kTestListAttribute);

    for (int i = 850; i < static_cast<int>(chip::app::kMaxSecureSduLengthBytes); i++)
    {
        CHIP_ERROR err = CHIP_NO_ERROR;
        TestWriteCallback writeCallback;

        ChipLogDetail(DataManagement, "Running iteration %d\n", i);

        gIterationCount = (uint32_t) i;

        app::WriteClient writeClient(&ctx.GetExchangeManager(), &writeCallback, Optional<uint16_t>::Missing());

        ByteSpan list[kTestListLength];
        for (uint8_t j = 0; j < kTestListLength; j++)
        {
            list[j] = ByteSpan(sByteSpanData, static_cast<uint32_t>(i));
        }

        err = writeClient.EncodeAttributeWritePayload(attributePath, app::DataModel::List<ByteSpan>(list, kTestListLength));
        if (err == CHIP_ERROR_NO_MEMORY || err == CHIP_ERROR_BUFFER_TOO_SMALL)
        {
            // This kind of error is expected.
            atLeastOneRequestFailed = true;
            continue;
        }

        atLeastOneRequestSent = true;

        // If we successfully encoded the attribute, then we must be able to send the message.
        writeClient.SendWriteRequest(sessionHandle);

        //
        // Service the IO + Engine till we get a ReportEnd callback on the client.
        // Since bugs can happen, we don't want this test to never stop, so create a ceiling for how many
        // times this can run without seeing expected results.
        //
        for (int j = 0; j < 10 && writeCallback.mOnDoneCount == 0; j++)
        {
            ctx.DrainAndServiceIO();
        }

        writeClient.SendWriteRequest(sessionHandle);

        NL_TEST_ASSERT(apSuite, writeCallback.mSuccessCount == kTestListLength);
        NL_TEST_ASSERT(apSuite, writeCallback.mErrorCount == 0);
        NL_TEST_ASSERT(apSuite, writeCallback.mOnDoneCount == 1);

        NL_TEST_ASSERT(apSuite, ctx.GetExchangeManager().GetNumActiveExchanges() == 0);

        //
        // Stop the test if we detected an error. Otherwise, it'll be difficult to read the logs.
        //
        if (apSuite->flagError)
        {
            break;
        }
    }
    NL_TEST_ASSERT(apSuite, ctx.GetExchangeManager().GetNumActiveExchanges() == 0);
    NL_TEST_ASSERT(apSuite, atLeastOneRequestSent && atLeastOneRequestFailed);
}

// clang-format off
const nlTest sTests[] =
{
    NL_TEST_DEF("TestListChunking", TestCommandInteraction::TestListChunking),
    NL_TEST_DEF("TestBadChunking", TestCommandInteraction::TestBadChunking),
    NL_TEST_SENTINEL()
};

// clang-format on

// clang-format off
nlTestSuite sSuite =
{
    "TestWriteChunking",
    &sTests[0],
    TestContext::InitializeAsync,
    TestContext::Finalize
};
// clang-format on

} // namespace

int TestReadChunkingTests()
{
    TestContext gContext;
    gSuite = &sSuite;
    nlTestRunner(&sSuite, &gContext);
    return (nlTestRunnerStats(&sSuite));
}

CHIP_REGISTER_TEST_SUITE(TestReadChunkingTests)
