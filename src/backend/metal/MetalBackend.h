// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_METAL_METALBACKEND_H_
#define BACKEND_METAL_METALBACKEND_H_

#include "nxt/nxtcpp.h"

#include <map>
#include <mutex>
#include <unordered_set>

#include "common/Buffer.h"
#include "common/BindGroup.h"
#include "common/BindGroupLayout.h"
#include "common/Device.h"
#include "common/CommandBuffer.h"
#include "common/InputState.h"
#include "common/Pipeline.h"
#include "common/PipelineLayout.h"
#include "common/Queue.h"
#include "common/Sampler.h"
#include "common/ShaderModule.h"
#include "common/Texture.h"
#include "common/ToBackend.h"

#include <type_traits>
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

namespace spirv_cross {
    class CompilerMSL;
}

namespace backend {
namespace metal {

    class BindGroup;
    class BindGroupLayout;
    class Buffer;
    class BufferView;
    class CommandBuffer;
    class InputState;
    class Pipeline;
    class PipelineLayout;
    class Queue;
    class Sampler;
    class ShaderModule;
    class Texture;
    class TextureView;

    struct MetalBackendTraits {
        using BindGroupType = BindGroup;
        using BindGroupLayoutType = BindGroupLayout;
        using BufferType = Buffer;
        using BufferViewType = BufferView;
        using CommandBufferType = CommandBuffer;
        using InputStateType = InputState;
        using PipelineType = Pipeline;
        using PipelineLayoutType = PipelineLayout;
        using QueueType = Queue;
        using SamplerType = Sampler;
        using ShaderModuleType = ShaderModule;
        using TextureType = Texture;
        using TextureViewType = TextureView;
    };

    template<typename T>
    auto ToBackend(T&& common) -> decltype(ToBackendBase<MetalBackendTraits>(common)) {
        return ToBackendBase<MetalBackendTraits>(common);
    }

    class Device : public DeviceBase {
        public:
            Device(id<MTLDevice> mtlDevice);
            ~Device();

            BindGroupBase* CreateBindGroup(BindGroupBuilder* builder) override;
            BindGroupLayoutBase* CreateBindGroupLayout(BindGroupLayoutBuilder* builder) override;
            BufferBase* CreateBuffer(BufferBuilder* builder) override;
            BufferViewBase* CreateBufferView(BufferViewBuilder* builder) override;
            CommandBufferBase* CreateCommandBuffer(CommandBufferBuilder* builder) override;
            InputStateBase* CreateInputState(InputStateBuilder* builder) override;
            PipelineBase* CreatePipeline(PipelineBuilder* builder) override;
            PipelineLayoutBase* CreatePipelineLayout(PipelineLayoutBuilder* builder) override;
            QueueBase* CreateQueue(QueueBuilder* builder) override;
            SamplerBase* CreateSampler(SamplerBuilder* builder) override;
            ShaderModuleBase* CreateShaderModule(ShaderModuleBuilder* builder) override;
            TextureBase* CreateTexture(TextureBuilder* builder) override;
            TextureViewBase* CreateTextureView(TextureViewBuilder* builder) override;

            void SetNextDrawable(id<CAMetalDrawable> drawable);
            void Present();

            id<MTLDevice> GetMTLDevice();
            id<MTLTexture> GetCurrentTexture();
            id<MTLTexture> GetCurrentDepthTexture();

            // NXT API
            void Reference();
            void Release();

        private:
            id<MTLDevice> mtlDevice = nil;
            id<MTLCommandQueue> commandQueue = nil;

            id<CAMetalDrawable> currentDrawable = nil;
            id<MTLTexture> currentTexture = nil;
            id<MTLTexture> currentDepthTexture = nil;
    };

    class BindGroup : public BindGroupBase {
        public:
            BindGroup(Device* device, BindGroupBuilder* builder);

        private:
            Device* device;
    };

    class BindGroupLayout : public BindGroupLayoutBase {
        public:
            BindGroupLayout(Device* device, BindGroupLayoutBuilder* builder);

        private:
            Device* device;
    };

    class Buffer : public BufferBase {
        public:
            Buffer(Device* device, BufferBuilder* builder);
            ~Buffer();

            id<MTLBuffer> GetMTLBuffer();
            std::mutex& GetMutex();

        private:
            void SetSubDataImpl(uint32_t start, uint32_t count, const uint32_t* data) override;

            Device* device;
            std::mutex mutex;
            id<MTLBuffer> mtlBuffer = nil;
    };

    class BufferView : public BufferViewBase {
        public:
            BufferView(Device* device, BufferViewBuilder* builder);

        private:
            Device* device;
    };

    class CommandBuffer : public CommandBufferBase {
        public:
            CommandBuffer(Device* device, CommandBufferBuilder* builder);
            ~CommandBuffer();

            void FillCommands(id<MTLCommandBuffer> commandBuffer, std::unordered_set<std::mutex*>* mutexes);

        private:
            Device* device;
            CommandIterator commands;
    };

    class InputState : public InputStateBase {
        public:
            InputState(Device* device, InputStateBuilder* builder);
            ~InputState();

            MTLVertexDescriptor* GetMTLVertexDescriptor();

        private:
            Device* device;
            MTLVertexDescriptor* mtlVertexDescriptor = nil;
    };

    class Pipeline : public PipelineBase {
        public:
            Pipeline(Device* device, PipelineBuilder* builder);
            ~Pipeline();

            void Encode(id<MTLRenderCommandEncoder> encoder);
            void Encode(id<MTLComputeCommandEncoder> encoder);
            MTLSize GetLocalWorkGroupSize() const;

        private:
            Device* device;

            id<MTLRenderPipelineState> mtlRenderPipelineState = nil;
            id<MTLDepthStencilState> mtlDepthStencilState = nil;

            id<MTLComputePipelineState> mtlComputePipelineState = nil;
            MTLSize localWorkgroupSize;
    };

    class PipelineLayout : public PipelineLayoutBase {
        public:
            PipelineLayout(Device* device, PipelineLayoutBuilder* builder);

            using BindingIndexInfo = std::array<std::array<uint32_t, kMaxBindingsPerGroup>, kMaxBindGroups>;
            const BindingIndexInfo& GetBindingIndexInfo(nxt::ShaderStage stage) const;

        private:
            Device* device;
            PerStage<BindingIndexInfo> indexInfo;
    };

    class Queue : public QueueBase {
        public:
            Queue(Device* device, QueueBuilder* builder);
            ~Queue();

            id<MTLCommandQueue> GetMTLCommandQueue();

            // NXT API
            void Submit(uint32_t numCommands, CommandBuffer* const * commands);

        private:
            Device* device;
            id<MTLCommandQueue> commandQueue = nil;
    };

    class Sampler : public SamplerBase {
        public:
            Sampler(Device* device, SamplerBuilder* builder);
            ~Sampler();

            id<MTLSamplerState> GetMTLSamplerState();

        private:
            Device* device;
            id<MTLSamplerState> mtlSamplerState = nil;
    };

    class ShaderModule : public ShaderModuleBase {
        public:
            ShaderModule(Device* device, ShaderModuleBuilder* builder);
            ~ShaderModule();

            id<MTLFunction> GetFunction(const char* functionName) const;
            MTLSize GetLocalWorkGroupSize(const std::string& entryPoint) const;

        private:
            Device* device;
            id<MTLLibrary> mtlLibrary = nil;
            spirv_cross::CompilerMSL* compiler = nullptr;
    };

    class Texture : public TextureBase {
        public:
            Texture(Device* device, TextureBuilder* builder);
            ~Texture();

            id<MTLTexture> GetMTLTexture();

        private:
            Device* device;
            id<MTLTexture> mtlTexture = nil;
    };

    class TextureView : public TextureViewBase {
        public:
            TextureView(Device* device, TextureViewBuilder* builder);

        private:
            Device* device;
    };

}
}

#endif // BACKEND_METAL_METALBACKEND_H_
