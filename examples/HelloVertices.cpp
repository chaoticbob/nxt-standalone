// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "Utils.h"

#include <unistd.h>
#include <vector>

nxt::Device device;

nxt::Buffer vertexBuffer;

nxt::Queue queue;
nxt::Pipeline pipeline;

void initBuffers() {
    static const float vertexData[12] = {
        0.0f, 0.5f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 1.0f,
    };
    vertexBuffer = device.CreateBufferBuilder()
        .SetUsage(nxt::BufferUsageBit::Mapped | nxt::BufferUsageBit::Vertex)
        .SetSize(sizeof(vertexData))
        .GetResult();
    vertexBuffer.SetSubData(0, sizeof(vertexData) / sizeof(uint32_t),
            reinterpret_cast<const uint32_t*>(vertexData));
}

void init() {
    nxtProcTable procs;
    GetProcTableAndDevice(&procs, &device);
    nxtSetProcs(&procs);

    queue = device.CreateQueueBuilder().GetResult();

    initBuffers();

    nxt::ShaderModule vsModule = CreateShaderModule(device, nxt::ShaderStage::Vertex, R"(
        #version 450
        layout(location = 0) in vec4 pos;
        void main() {
            gl_Position = pos;
        })"
    );

    nxt::ShaderModule fsModule = CreateShaderModule(device, nxt::ShaderStage::Fragment, R"(
        #version 450
        out vec4 fragColor;
        void main() {
            fragColor = vec4(1.0, 0.0, 0.0, 1.0);
        })"
    );

    auto inputState = device.CreateInputStateBuilder()
        .SetAttribute(0, 0, nxt::VertexFormat::FloatR32G32B32A32, 0)
        .SetInput(0, 4 * sizeof(float), nxt::InputStepMode::Vertex)
        .GetResult();

    pipeline = device.CreatePipelineBuilder()
        .SetStage(nxt::ShaderStage::Vertex, vsModule, "main")
        .SetStage(nxt::ShaderStage::Fragment, fsModule, "main")
        .SetInputState(inputState)
        .GetResult();
}

void frame() {
    static const uint32_t vertexBufferOffsets[1] = {0};
    nxt::CommandBuffer commands = device.CreateCommandBufferBuilder()
        .SetPipeline(pipeline)
        .SetVertexBuffers(0, 1, &vertexBuffer, vertexBufferOffsets)
        .DrawArrays(3, 1, 0, 0)
        .GetResult();

    queue.Submit(1, &commands);
    SwapBuffers();
}

int main(int argc, const char* argv[]) {
    if (!InitUtils(argc, argv)) {
        return 1;
    }
    init();

    while (!ShouldQuit()) {
        frame();
        usleep(16000);
    }

    // TODO release stuff
}
