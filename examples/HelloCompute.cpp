// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "Utils.h"

#include <string.h>
#include <unistd.h>

nxt::Device device;
nxt::Queue queue;
nxt::Buffer buffer;
nxt::Pipeline renderPipeline;
nxt::BindGroup renderBindGroup;
nxt::Pipeline computePipeline;
nxt::BindGroup computeBindGroup;

void init() {
    nxtProcTable procs;
    GetProcTableAndDevice(&procs, &device);
    nxtSetProcs(&procs);

    queue = device.CreateQueueBuilder().GetResult();

    struct {uint32_t a; float b;} s;
    memset(&s, sizeof(s), 0);
    buffer = device.CreateBufferBuilder()
        .SetUsage(nxt::BufferUsageBit::Uniform | nxt::BufferUsageBit::Storage | nxt::BufferUsageBit::Mapped)
        .SetSize(sizeof(s))
        .GetResult();
    buffer.SetSubData(0, sizeof(s) / sizeof(uint32_t), reinterpret_cast<uint32_t*>(&s));

    nxt::BufferView view = buffer.CreateBufferViewBuilder()
        .SetExtent(0, sizeof(s))
        .GetResult();

    {
        nxt::ShaderModule module = CreateShaderModule(device, nxt::ShaderStage::Compute, R"(
            #version 450
            layout(set = 0, binding = 0) buffer myBlock {
                int a;
                float b;
            } myStorage;
            void main() {
                myStorage.a = (myStorage.a + 1) % 256;
                myStorage.b = mod((myStorage.b + 0.02), 1.0);
            })"
        );

        nxt::BindGroupLayout bgl = device.CreateBindGroupLayoutBuilder()
            .SetBindingsType(nxt::ShaderStageBit::Compute, nxt::BindingType::StorageBuffer, 0, 1)
            .GetResult();

        nxt::PipelineLayout pl = device.CreatePipelineLayoutBuilder()
            .SetBindGroupLayout(0, bgl)
            .GetResult();

        computePipeline = device.CreatePipelineBuilder()
            .SetLayout(pl)
            .SetStage(nxt::ShaderStage::Compute, module, "main")
            .GetResult();

        computeBindGroup = device.CreateBindGroupBuilder()
            .SetLayout(bgl)
            .SetUsage(nxt::BindGroupUsage::Frozen)
            .SetBufferViews(0, 1, &view)
            .GetResult();
    }

    {
        nxt::ShaderModule vsModule = CreateShaderModule(device, nxt::ShaderStage::Vertex, R"(
            #version 450
            const vec2 pos[3] = vec2[3](vec2(0.0f, 0.5f), vec2(-0.5f, -0.5f), vec2(0.5f, -0.5f));
            void main() {
                gl_Position = vec4(pos[gl_VertexIndex], 0.5, 1.0);
            })"
        );

        nxt::ShaderModule fsModule = CreateShaderModule(device, nxt::ShaderStage::Fragment, R"(
            #version 450
            layout(set = 0, binding = 0) uniform myBlock {
                int a;
                float b;
            } myUbo;
            out vec4 fragColor;
            void main() {
                fragColor = vec4(1.0, myUbo.a / 255.0, myUbo.b, 1.0);
            })"
        );

        nxt::BindGroupLayout bgl = device.CreateBindGroupLayoutBuilder()
            .SetBindingsType(nxt::ShaderStageBit::Fragment, nxt::BindingType::UniformBuffer, 0, 1)
            .GetResult();

        nxt::PipelineLayout pl = device.CreatePipelineLayoutBuilder()
            .SetBindGroupLayout(0, bgl)
            .GetResult();

        renderPipeline = device.CreatePipelineBuilder()
            .SetLayout(pl)
            .SetStage(nxt::ShaderStage::Vertex, vsModule, "main")
            .SetStage(nxt::ShaderStage::Fragment, fsModule, "main")
            .GetResult();

        renderBindGroup = device.CreateBindGroupBuilder()
            .SetLayout(bgl)
            .SetUsage(nxt::BindGroupUsage::Frozen)
            .SetBufferViews(0, 1, &view)
            .GetResult();
    }
}

void frame() {
    nxt::CommandBuffer commands = device.CreateCommandBufferBuilder()
        .SetPipeline(computePipeline)
        .SetBindGroup(0, computeBindGroup)
        .Dispatch(1, 1, 1)

        .SetPipeline(renderPipeline)
        .SetBindGroup(0, renderBindGroup)
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
