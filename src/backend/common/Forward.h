// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_COMMON_FORWARD_H_
#define BACKEND_COMMON_FORWARD_H_

#include <cassert>
#include <cstdint>

#define ASSERT assert

namespace backend {

    class BindGroupBase;
    class BindGroupBuilder;
    class BindGroupLayoutBase;
    class BindGroupLayoutBuilder;
    class BufferBase;
    class BufferBuilder;
    class BufferViewBase;
    class BufferViewBuilder;
    class CommandBufferBase;
    class CommandBufferBuilder;
    class InputStateBase;
    class InputStateBuilder;
    class PipelineBase;
    class PipelineBuilder;
    class PipelineLayoutBase;
    class PipelineLayoutBuilder;
    class QueueBase;
    class QueueBuilder;
    class SamplerBase;
    class SamplerBuilder;
    class ShaderModuleBase;
    class ShaderModuleBuilder;
    class TextureBase;
    class TextureBuilder;
    class TextureViewBase;
    class TextureViewBuilder;

    class DeviceBase;

    template<typename T>
    class Ref;

    template<typename T>
    class PerStage;

    // TODO(cwallez@chromium.org): where should constants live?
    static constexpr uint32_t kMaxPushConstants = 32u;
    static constexpr uint32_t kMaxBindGroups = 4u;
    static constexpr uint32_t kMaxBindingsPerGroup = 16u; // TODO(cwallez@chromium.org): investigate bindgroup limits
    static constexpr uint32_t kMaxVertexAttributes = 16u;
    static constexpr uint32_t kMaxVertexInputs = 16u;
    static constexpr uint32_t kNumStages = 3;

    enum PushConstantType : uint8_t;
}

#endif // BACKEND_COMMON_FORWARD_H_
