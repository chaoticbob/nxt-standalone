// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_COMMON_DEVICEBASE_H_
#define BACKEND_COMMON_DEVICEBASE_H_

#include "common/Forward.h"
#include "common/RefCounted.h"

#include "nxt/nxtcpp.h"

namespace backend {

    using ErrorCallback = void (*)(const char* errorMessage, void* userData);

    class DeviceBase {
        public:
            DeviceBase();
            ~DeviceBase();

            void HandleError(const char* message);
            void RegisterErrorCallback(ErrorCallback callback, void* userData);

            virtual BindGroupBase* CreateBindGroup(BindGroupBuilder* builder) = 0;
            virtual BindGroupLayoutBase* CreateBindGroupLayout(BindGroupLayoutBuilder* builder) = 0;
            virtual BufferBase* CreateBuffer(BufferBuilder* builder) = 0;
            virtual BufferViewBase* CreateBufferView(BufferViewBuilder* builder) = 0;
            virtual CommandBufferBase* CreateCommandBuffer(CommandBufferBuilder* builder) = 0;
            virtual InputStateBase* CreateInputState(InputStateBuilder* builder) = 0;
            virtual PipelineBase* CreatePipeline(PipelineBuilder* builder) = 0;
            virtual PipelineLayoutBase* CreatePipelineLayout(PipelineLayoutBuilder* builder) = 0;
            virtual QueueBase* CreateQueue(QueueBuilder* builder) = 0;
            virtual SamplerBase* CreateSampler(SamplerBuilder* builder) = 0;
            virtual ShaderModuleBase* CreateShaderModule(ShaderModuleBuilder* builder) = 0;
            virtual TextureBase* CreateTexture(TextureBuilder* builder) = 0;
            virtual TextureViewBase* CreateTextureView(TextureViewBuilder* builder) = 0;

            // Many NXT objects are completely immutable once created which means that if two
            // builders are given the same arguments, they can return the same object. Reusing
            // objects will help make comparisons between objects by a single pointer comparison.
            //
            // Technically no object is immutable as they have a reference count, and an
            // application with reference-counting issues could "see" that objects are reused.
            // This is solved by automatic-reference counting, and also the fact that when using
            // the client-server wire every creation will get a different proxy object, with a
            // different reference count.
            //
            // When trying to create an object, we give both the builder and an example of what
            // the built object will be, the "blueprint". The blueprint is just a FooBase object
            // instead of a backend Foo object. If the blueprint doesn't match an object in the
            // cache, then the builder is used to make a new object.
            BindGroupLayoutBase* GetOrCreateBindGroupLayout(const BindGroupLayoutBase* blueprint, BindGroupLayoutBuilder* builder);
            void UncacheBindGroupLayout(BindGroupLayoutBase* obj);

            // NXT API
            BindGroupBuilder* CreateBindGroupBuilder();
            BindGroupLayoutBuilder* CreateBindGroupLayoutBuilder();
            BufferBuilder* CreateBufferBuilder();
            BufferViewBuilder* CreateBufferViewBuilder();
            CommandBufferBuilder* CreateCommandBufferBuilder();
            InputStateBuilder* CreateInputStateBuilder();
            PipelineBuilder* CreatePipelineBuilder();
            PipelineLayoutBuilder* CreatePipelineLayoutBuilder();
            QueueBuilder* CreateQueueBuilder();
            SamplerBuilder* CreateSamplerBuilder();
            ShaderModuleBuilder* CreateShaderModuleBuilder();
            TextureBuilder* CreateTextureBuilder();

            void CopyBindGroups(uint32_t start, uint32_t count, BindGroupBase* source, BindGroupBase* target);

        private:
            // The object caches aren't exposed in the header as they would require a lot of
            // additional includes.
            struct Caches;
            Caches* caches = nullptr;

            ErrorCallback errorCallback = nullptr;
            void* errorUserData = nullptr;
    };

}

#endif // BACKEND_COMMON_DEVICEBASE_H_
