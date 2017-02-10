// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "Utils.h"

#include <array>
#include <cstring>
#include <random>
#include <unistd.h>

#include <glm/glm.hpp>

nxt::Device device;
nxt::Queue queue;

nxt::Buffer modelBuffer;
std::array<nxt::Buffer, 2> particleBuffers;

nxt::Pipeline renderPipeline;

nxt::Buffer updateParams;
nxt::Pipeline updatePipeline;
std::array<nxt::BindGroup, 2> updateBGs;

std::array<nxt::CommandBuffer, 2> commandBuffers;

size_t pingpong = 0;

static const uint32_t kNumParticles = 1000;

struct Particle {
    glm::vec2 pos;
    glm::vec2 vel;
};

struct SimParams {
    float deltaT;
    float rule1Distance;
    float rule2Distance;
    float rule3Distance;
    float rule1Scale;
    float rule2Scale;
    float rule3Scale;
    int particleCount;
};

void initBuffers() {
    glm::vec2 model[3] = {
        {-0.01, -0.02},
        {0.01, -0.02},
        {0.00, 0.02},
    };
    modelBuffer = device.CreateBufferBuilder()
        .SetUsage(nxt::BufferUsageBit::Vertex | nxt::BufferUsageBit::Mapped)
        .SetSize(sizeof(model))
        .GetResult();
    modelBuffer.SetSubData(0, sizeof(model) / sizeof(uint32_t),
            reinterpret_cast<uint32_t*>(model));

    SimParams params = { 0.04, 0.1, 0.025, 0.025, 0.02, 0.05, 0.005, kNumParticles };
    updateParams = device.CreateBufferBuilder()
        .SetUsage(nxt::BufferUsageBit::Uniform | nxt::BufferUsageBit::Mapped)
        .SetSize(sizeof(SimParams))
        .GetResult();
    updateParams.SetSubData(0, sizeof(SimParams) / sizeof(uint32_t),
            reinterpret_cast<uint32_t*>(&params));

    std::vector<Particle> initialParticles(kNumParticles);
    {
        std::mt19937 generator;
        std::uniform_real_distribution<float> dist(-1.0f, 1.0f);
        for (auto& p : initialParticles)
        {
            p.pos = glm::vec2(dist(generator), dist(generator));
            p.vel = glm::vec2(dist(generator), dist(generator)) * 0.1f;
        }
    }

    for (int i = 0; i < 2; i++) {
        particleBuffers[i] = device.CreateBufferBuilder()
            .SetUsage(nxt::BufferUsageBit::Vertex | nxt::BufferUsageBit::Storage | nxt::BufferUsageBit::Mapped)
            .SetSize(sizeof(Particle) * kNumParticles)
            .GetResult();

        particleBuffers[i].SetSubData(0,
            sizeof(Particle) * kNumParticles / sizeof(uint32_t),
            reinterpret_cast<uint32_t*>(initialParticles.data()));
    }
}

void initRender() {
    nxt::ShaderModule vsModule = CreateShaderModule(device, nxt::ShaderStage::Vertex, R"(
        #version 450
        layout(location = 0) in vec2 a_particlePos;
        layout(location = 1) in vec2 a_particleVel;
        layout(location = 2) in vec2 a_pos;
        void main() {
            float angle = -atan(a_particleVel.x, a_particleVel.y);
            vec2 pos = vec2(a_pos.x * cos(angle) - a_pos.y * sin(angle),
                            a_pos.x * sin(angle) + a_pos.y * cos(angle));
            gl_Position = vec4(pos + a_particlePos, 0, 1);
        }
    )");

    nxt::ShaderModule fsModule = CreateShaderModule(device, nxt::ShaderStage::Fragment, R"(
        #version 450
        out vec4 fragColor;
        void main() {
            fragColor = vec4(1.0);
        }
    )");

    nxt::InputState inputState = device.CreateInputStateBuilder()
        .SetAttribute(0, 0, nxt::VertexFormat::FloatR32G32, offsetof(Particle, pos))
        .SetAttribute(1, 0, nxt::VertexFormat::FloatR32G32, offsetof(Particle, vel))
        .SetInput(0, sizeof(Particle), nxt::InputStepMode::Instance)
        .SetAttribute(2, 1, nxt::VertexFormat::FloatR32G32, 0)
        .SetInput(1, sizeof(glm::vec2), nxt::InputStepMode::Vertex)
        .GetResult();

    renderPipeline = device.CreatePipelineBuilder()
        .SetStage(nxt::ShaderStage::Vertex, vsModule, "main")
        .SetStage(nxt::ShaderStage::Fragment, fsModule, "main")
        .SetInputState(inputState)
        .GetResult();
}

void initSim() {
    nxt::ShaderModule module = CreateShaderModule(device, nxt::ShaderStage::Compute, R"(
        #version 450

        struct Particle {
            vec2 pos;
            vec2 vel;
        };

        layout(std140, set = 0, binding = 0) uniform SimParams {
            float deltaT;
            float rule1Distance;
            float rule2Distance;
            float rule3Distance;
            float rule1Scale;
            float rule2Scale;
            float rule3Scale;
            int particleCount;
        } params;

        layout(std140, set = 0, binding = 1) buffer ParticlesA {
            Particle particlesA[1000];
        };

        layout(std140, set = 0, binding = 2) buffer ParticlesB {
            Particle particlesB[1000];
        };

        void main() {
            // https://github.com/austinEng/Project6-Vulkan-Flocking/blob/master/data/shaders/computeparticles/particle.comp

            uint index = gl_GlobalInvocationID.x;
            if (index >= params.particleCount) { return; }

            vec2 vPos = particlesA[index].pos;
            vec2 vVel = particlesA[index].vel;

            vec2 cMass = vec2(0.0, 0.0);
            vec2 cVel = vec2(0.0, 0.0);
            vec2 colVel = vec2(0.0, 0.0);
            int cMassCount = 0;
            int cVelCount = 0;

            vec2 pos;
            vec2 vel;
            for (int i = 0; i < params.particleCount; ++i) {
                if (i == index) { continue; }
                pos = particlesA[i].pos.xy;
                vel = particlesA[i].vel.xy;

                if (distance(pos, vPos) < params.rule1Distance) {
                    cMass += pos;
                    cMassCount++;
                }
                if (distance(pos, vPos) < params.rule2Distance) {
                    colVel -= (pos - vPos);
                }
                if (distance(pos, vPos) < params.rule3Distance) {
                    cVel += vel;
                    cVelCount++;
                }
            }
            if (cMassCount > 0) {
                cMass = cMass / cMassCount - vPos;
            }
            if (cVelCount > 0) {
                cVel = cVel / cVelCount;
            }

            vVel += cMass * params.rule1Scale + colVel * params.rule2Scale + cVel * params.rule3Scale;

            // clamp velocity for a more pleasing simulation.
            vVel = normalize(vVel) * clamp(length(vVel), 0.0, 0.1);

            // kinematic update
            vPos += vVel * params.deltaT;

            // Wrap around boundary
            if (vPos.x < -1.0) vPos.x = 1.0;
            if (vPos.x > 1.0) vPos.x = -1.0;
            if (vPos.y < -1.0) vPos.y = 1.0;
            if (vPos.y > 1.0) vPos.y = -1.0;

            particlesB[index].pos = vPos;

            // Write back
            particlesB[index].vel = vVel;
        }
    )");

    nxt::BindGroupLayout bgl = device.CreateBindGroupLayoutBuilder()
        .SetBindingsType(nxt::ShaderStageBit::Compute, nxt::BindingType::UniformBuffer, 0, 1)
        .SetBindingsType(nxt::ShaderStageBit::Compute, nxt::BindingType::StorageBuffer, 1, 2)
        .GetResult();

    nxt::PipelineLayout pl = device.CreatePipelineLayoutBuilder()
        .SetBindGroupLayout(0, bgl)
        .GetResult();

    updatePipeline = device.CreatePipelineBuilder()
        .SetLayout(pl)
        .SetStage(nxt::ShaderStage::Compute, module, "main")
        .GetResult();

    nxt::BufferView updateParamsView = updateParams.CreateBufferViewBuilder()
        .SetExtent(0, sizeof(SimParams))
        .GetResult();

    std::array<nxt::BufferView, 2> views;
    for (uint32_t i = 0; i < 2; ++i) {
        views[i] = particleBuffers[i].CreateBufferViewBuilder()
            .SetExtent(0, kNumParticles * sizeof(Particle))
            .GetResult();
    }

    for (uint32_t i = 0; i < 2; ++i) {
        updateBGs[i] = device.CreateBindGroupBuilder()
            .SetLayout(bgl)
            .SetUsage(nxt::BindGroupUsage::Frozen)
            .SetBufferViews(0, 1, &updateParamsView)
            .SetBufferViews(1, 1, &views[i])
            .SetBufferViews(2, 1, &views[(i + 1) % 2])
            .GetResult();
    }
}

void initCommandBuffers() {
    static const uint32_t zeroOffsets[1] = {0};
    for (int i = 0; i < 2; ++i) {
        commandBuffers[i] = device.CreateCommandBufferBuilder()
            .SetPipeline(updatePipeline)
            .SetBindGroup(0, updateBGs[i])
            .Dispatch(kNumParticles, 1, 1)

            .SetPipeline(renderPipeline)
            .SetVertexBuffers(0, 1, &particleBuffers[(i + 1) % 2], zeroOffsets)
            .SetVertexBuffers(1, 1, &modelBuffer, zeroOffsets)
            .DrawArrays(3, kNumParticles, 0, 0)

            .GetResult();
    }
}

void init() {
    nxtProcTable procs;
    GetProcTableAndDevice(&procs, &device);
    nxtSetProcs(&procs);

    queue = device.CreateQueueBuilder().GetResult();

    initBuffers();
    initRender();
    initSim();
    initCommandBuffers();
}

void frame() {
    queue.Submit(1, &commandBuffers[pingpong]);
    SwapBuffers();

    pingpong = (pingpong + 1) % 2;
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
