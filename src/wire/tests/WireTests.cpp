// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gtest/gtest.h"
#include "mock/mock_nxt.h"

#include "TerribleCommandBuffer.h"
#include "Wire.h"

using namespace testing;

class WireTests : public Test {
    protected:
        void SetUp() override {
            nxtProcTable mockProcs;
            nxtDevice mockDevice;
            api.GetProcTableAndDevice(&mockProcs, &mockDevice);

            wireServer = server::CreateCommandHandler(mockDevice, mockProcs);

            cmdBuf = new TerribleCommandBuffer(wireServer);

            nxtDevice clientDevice;
            nxtProcTable clientProcs;
            client::NewClientDevice(&clientProcs, &clientDevice, cmdBuf);

            nxtSetProcs(&clientProcs);
            device = clientDevice;
            apiDevice = mockDevice;
        }

        void TearDown() override {
            nxtSetProcs(nullptr);
            delete wireServer;
            delete cmdBuf;
        }

        void Flush() {
            cmdBuf->Flush();
        }

        MockProcTable api;
        nxtDevice apiDevice;
        nxtDevice device;

    private:
        server::CommandHandler* wireServer = nullptr;
        TerribleCommandBuffer* cmdBuf = nullptr;
};

// One call gets forwarded correctly.
TEST_F(WireTests, CallForwarded) {
    nxtCommandBufferBuilder builder = nxtDeviceCreateCommandBufferBuilder(device);

    nxtCommandBufferBuilder apiCmdBufBuilder = api.GetNewCommandBufferBuilder();
    EXPECT_CALL(api, DeviceCreateCommandBufferBuilder(apiDevice))
        .WillOnce(Return(apiCmdBufBuilder));

    Flush();
}

// Test that calling methods on a new object works as expected.
TEST_F(WireTests, CreateThenCall) {
    nxtCommandBufferBuilder builder = nxtDeviceCreateCommandBufferBuilder(device);
    nxtCommandBuffer cmdBuf = nxtCommandBufferBuilderGetResult(builder);

    nxtCommandBufferBuilder apiCmdBufBuilder = api.GetNewCommandBufferBuilder();
    EXPECT_CALL(api, DeviceCreateCommandBufferBuilder(apiDevice))
        .WillOnce(Return(apiCmdBufBuilder));

    nxtCommandBuffer apiCmdBuf = api.GetNewCommandBuffer();
    EXPECT_CALL(api, CommandBufferBuilderGetResult(apiCmdBufBuilder))
        .WillOnce(Return(apiCmdBuf));

    Flush();
}

// Test that client reference/release do not call the backend API.
TEST_F(WireTests, RefCountKeptInClient) {
    nxtCommandBufferBuilder builder = nxtDeviceCreateCommandBufferBuilder(device);

    nxtCommandBufferBuilderReference(builder);
    nxtCommandBufferBuilderRelease(builder);

    nxtCommandBufferBuilder apiCmdBufBuilder = api.GetNewCommandBufferBuilder();
    EXPECT_CALL(api, DeviceCreateCommandBufferBuilder(apiDevice))
        .WillOnce(Return(apiCmdBufBuilder));

    Flush();
}

// Test that client reference/release do not call the backend API.
TEST_F(WireTests, ReleaseCalledOnRefCount0) {
    nxtCommandBufferBuilder builder = nxtDeviceCreateCommandBufferBuilder(device);

    nxtCommandBufferBuilderRelease(builder);

    nxtCommandBufferBuilder apiCmdBufBuilder = api.GetNewCommandBufferBuilder();
    EXPECT_CALL(api, DeviceCreateCommandBufferBuilder(apiDevice))
        .WillOnce(Return(apiCmdBufBuilder));

    EXPECT_CALL(api, CommandBufferBuilderRelease(apiCmdBufBuilder));

    Flush();
}

TEST_F(WireTests, ObjectAsValueArgument) {
    // Create pipeline
    nxtPipelineBuilder pipelineBuilder = nxtDeviceCreatePipelineBuilder(device);
    nxtPipeline pipeline = nxtPipelineBuilderGetResult(pipelineBuilder);

    nxtPipelineBuilder apiPipelineBuilder = api.GetNewPipelineBuilder();
    EXPECT_CALL(api, DeviceCreatePipelineBuilder(apiDevice))
        .WillOnce(Return(apiPipelineBuilder));

    nxtPipeline apiPipeline = api.GetNewPipeline();
    EXPECT_CALL(api, PipelineBuilderGetResult(apiPipelineBuilder))
        .WillOnce(Return(apiPipeline));

    // Create command buffer builder, setting pipeline
    nxtCommandBufferBuilder cmdBufBuilder = nxtDeviceCreateCommandBufferBuilder(device);
    nxtCommandBufferBuilderSetPipeline(cmdBufBuilder, pipeline);

    nxtCommandBufferBuilder apiCmdBufBuilder = api.GetNewCommandBufferBuilder();
    EXPECT_CALL(api, DeviceCreateCommandBufferBuilder(apiDevice))
        .WillOnce(Return(apiCmdBufBuilder));

    EXPECT_CALL(api, CommandBufferBuilderSetPipeline(apiCmdBufBuilder, apiPipeline));

    Flush();
}

TEST_F(WireTests, OneObjectAsPointerArgument) {
    // Create command buffer
    nxtCommandBufferBuilder cmdBufBuilder = nxtDeviceCreateCommandBufferBuilder(device);
    nxtCommandBuffer cmdBuf = nxtCommandBufferBuilderGetResult(cmdBufBuilder);

    nxtCommandBufferBuilder apiCmdBufBuilder = api.GetNewCommandBufferBuilder();
    EXPECT_CALL(api, DeviceCreateCommandBufferBuilder(apiDevice))
        .WillOnce(Return(apiCmdBufBuilder));

    nxtCommandBuffer apiCmdBuf = api.GetNewCommandBuffer();
    EXPECT_CALL(api, CommandBufferBuilderGetResult(apiCmdBufBuilder))
        .WillOnce(Return(apiCmdBuf));

    // Create queue
    nxtQueueBuilder queueBuilder = nxtDeviceCreateQueueBuilder(device);
    nxtQueue queue = nxtQueueBuilderGetResult(queueBuilder);

    nxtQueueBuilder apiQueueBuilder = api.GetNewQueueBuilder();
    EXPECT_CALL(api, DeviceCreateQueueBuilder(apiDevice))
        .WillOnce(Return(apiQueueBuilder));

    nxtQueue apiQueue = api.GetNewQueue();
    EXPECT_CALL(api, QueueBuilderGetResult(apiQueueBuilder))
        .WillOnce(Return(apiQueue));

    // Submit command buffer
    nxtQueueSubmit(queue, 1, &cmdBuf);

    EXPECT_CALL(api, QueueSubmit(apiQueue, 1, Pointee(apiCmdBuf)));

    Flush();
}

// TODO
//  - Test values work
//  - Test multiple objects as value work
//  - Object creation, then calls do nothing after error on builder
//  - Object creation then error then create object, then should do nothing.
