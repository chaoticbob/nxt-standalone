// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "Utils.h"

#include <unistd.h>
#include <vector>

nxt::Device device;

nxt::Buffer indexBuffer;
nxt::Buffer vertexBuffer;

nxt::Texture texture;
nxt::Sampler sampler;

nxt::Queue queue;
nxt::Pipeline pipeline;
nxt::BindGroup bindGroup;

void initBuffers() {
    static const uint32_t indexData[3] = {
        0, 1, 2,
    };
    indexBuffer = device.CreateBufferBuilder()
        .SetUsage(nxt::BufferUsageBit::Mapped | nxt::BufferUsageBit::Index)
        .SetSize(sizeof(indexData))
        .GetResult();
    indexBuffer.SetSubData(0, sizeof(indexData) / sizeof(uint32_t), indexData);

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

void initTextures() {
    texture = device.CreateTextureBuilder()
        .SetDimension(nxt::TextureDimension::e2D)
        .SetExtent(1024, 1024, 1)
        .SetFormat(nxt::TextureFormat::R8G8B8A8Unorm)
        .SetMipLevels(1)
        .SetUsage(nxt::TextureUsageBit::Sampled | nxt::TextureUsageBit::TransferDst)
        .GetResult();

    sampler = device.CreateSamplerBuilder()
        .SetFilterMode(nxt::FilterMode::Linear, nxt::FilterMode::Linear, nxt::FilterMode::Linear)
        .GetResult();

    // Initialize the texture with arbitrary data until we can load images
    std::vector<uint8_t> data(4 * 1024 * 1024, 0);
    for (size_t i = 0; i < data.size(); ++i) {
        data[i] = i % 253;
    }

    nxt::Buffer stagingBuffer = device.CreateBufferBuilder()
        .SetUsage(nxt::BufferUsageBit::Mapped | nxt::BufferUsageBit::TransferSrc)
        .SetSize(data.size())
        .GetResult();
    stagingBuffer.SetSubData(0, data.size() / sizeof(uint32_t), reinterpret_cast<uint32_t*>(data.data()));

    nxt::CommandBuffer copy = device.CreateCommandBufferBuilder()
        .CopyBufferToTexture(stagingBuffer, texture, 0, 0, 0, 1024, 1024, 1, 0)
        .GetResult();

    queue.Submit(1, &copy);
}

void init() {
    nxtProcTable procs;
    GetProcTableAndDevice(&procs, &device);
    nxtSetProcs(&procs);

    queue = device.CreateQueueBuilder().GetResult();

    initBuffers();
    initTextures();

    nxt::ShaderModule vsModule = CreateShaderModule(device, nxt::ShaderStage::Vertex, R"(
        #version 450
        layout(location = 0) in vec4 pos;
        void main() {
            gl_Position = pos;
        })"
    );

    nxt::ShaderModule fsModule = CreateShaderModule(device, nxt::ShaderStage::Fragment, R"(
        #version 450
        layout(set = 0, binding = 0) uniform sampler mySampler;
        layout(set = 0, binding = 1) uniform texture2D myTexture;

        out vec4 fragColor;
        void main() {
            fragColor = texture(sampler2D(myTexture, mySampler), gl_FragCoord.xy / vec2(640.0, 480.0));
        })"
    );

    auto inputState = device.CreateInputStateBuilder()
        .SetAttribute(0, 0, nxt::VertexFormat::FloatR32G32B32A32, 0)
        .SetInput(0, 4 * sizeof(float), nxt::InputStepMode::Vertex)
        .GetResult();

    nxt::BindGroupLayout bgl = device.CreateBindGroupLayoutBuilder()
        .SetBindingsType(nxt::ShaderStageBit::Fragment, nxt::BindingType::Sampler, 0, 1)
        .SetBindingsType(nxt::ShaderStageBit::Fragment, nxt::BindingType::SampledTexture, 1, 1)
        .GetResult();

    nxt::PipelineLayout pl = device.CreatePipelineLayoutBuilder()
        .SetBindGroupLayout(0, bgl)
        .GetResult();

    pipeline = device.CreatePipelineBuilder()
        .SetLayout(pl)
        .SetStage(nxt::ShaderStage::Vertex, vsModule, "main")
        .SetStage(nxt::ShaderStage::Fragment, fsModule, "main")
        .SetInputState(inputState)
        .GetResult();

    nxt::TextureView view = texture.CreateTextureViewBuilder().GetResult();

    bindGroup = device.CreateBindGroupBuilder()
        .SetLayout(bgl)
        .SetUsage(nxt::BindGroupUsage::Frozen)
        .SetSamplers(0, 1, &sampler)
        .SetTextureViews(1, 1, &view)
        .GetResult();
}

struct {uint32_t a; float b;} s;
void frame() {
    s.a = (s.a + 1) % 256;
    s.b += 0.02;
    if (s.b >= 1.0f) {s.b = 0.0f;}
    static const uint32_t vertexBufferOffsets[1] = {0};
    nxt::CommandBuffer commands = device.CreateCommandBufferBuilder()
        .SetPipeline(pipeline)
        .SetBindGroup(0, bindGroup)
        .SetVertexBuffers(0, 1, &vertexBuffer, vertexBufferOffsets)
        .SetIndexBuffer(indexBuffer, 0, nxt::IndexFormat::Uint32)
        .DrawElements(3, 1, 0, 0)
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
