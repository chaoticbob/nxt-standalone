// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "Device.h"

#include "BindGroup.h"
#include "BindGroupLayout.h"
#include "Buffer.h"
#include "CommandBuffer.h"
#include "InputState.h"
#include "Pipeline.h"
#include "PipelineLayout.h"
#include "Queue.h"
#include "Sampler.h"
#include "ShaderModule.h"
#include "Texture.h"

#include <unordered_set>

namespace backend {

    void RegisterSynchronousErrorCallback(nxtDevice device, ErrorCallback callback, void* userData) {
        auto deviceBase = reinterpret_cast<DeviceBase*>(device);
        deviceBase->RegisterErrorCallback(callback, userData);
    }

    // DeviceBase::Caches

    // The caches are unordered_sets of pointers with special hash and compare functions
    // to compare the value of the objects, instead of the pointers.
    using BindGroupLayoutCache = std::unordered_set<BindGroupLayoutBase*, BindGroupLayoutCacheFuncs, BindGroupLayoutCacheFuncs>;

    struct DeviceBase::Caches {
        BindGroupLayoutCache bindGroupLayouts;
    };

    // DeviceBase

    DeviceBase::DeviceBase() {
        caches = new DeviceBase::Caches();
    }

    DeviceBase::~DeviceBase() {
        delete caches;
    }

    void DeviceBase::HandleError(const char* message) {
        if (errorCallback) {
            errorCallback(message, errorUserData);
        }
    }

    void DeviceBase::RegisterErrorCallback(ErrorCallback callback, void* userData) {
        this->errorCallback = callback;
        this->errorUserData = userData;
    }

    BindGroupLayoutBase* DeviceBase::GetOrCreateBindGroupLayout(const BindGroupLayoutBase* blueprint, BindGroupLayoutBuilder* builder) {
        // The blueprint is only used to search in the cache and is not modified. However cached
        // objects can be modified, and unordered_set cannot search for a const pointer in a non
        // const pointer set. That's why we do a const_cast here, but the blueprint won't be
        // modified.
        auto iter = caches->bindGroupLayouts.find(const_cast<BindGroupLayoutBase*>(blueprint));
        if (iter != caches->bindGroupLayouts.end()) {
            return *iter;
        }

        BindGroupLayoutBase* backendObj = CreateBindGroupLayout(builder);
        caches->bindGroupLayouts.insert(backendObj);
        return backendObj;
    }

    void DeviceBase::UncacheBindGroupLayout(BindGroupLayoutBase* obj) {
        caches->bindGroupLayouts.erase(obj);
    }

    BindGroupBuilder* DeviceBase::CreateBindGroupBuilder() {
        return new BindGroupBuilder(this);
    }
    BindGroupLayoutBuilder* DeviceBase::CreateBindGroupLayoutBuilder() {
        return new BindGroupLayoutBuilder(this);
    }
    BufferBuilder* DeviceBase::CreateBufferBuilder() {
        return new BufferBuilder(this);
    }
    CommandBufferBuilder* DeviceBase::CreateCommandBufferBuilder() {
        return new CommandBufferBuilder(this);
    }
    InputStateBuilder* DeviceBase::CreateInputStateBuilder() {
        return new InputStateBuilder(this);
    }
    PipelineBuilder* DeviceBase::CreatePipelineBuilder() {
        return new PipelineBuilder(this);
    }
    PipelineLayoutBuilder* DeviceBase::CreatePipelineLayoutBuilder() {
        return new PipelineLayoutBuilder(this);
    }
    QueueBuilder* DeviceBase::CreateQueueBuilder() {
        return new QueueBuilder(this);
    }
    SamplerBuilder* DeviceBase::CreateSamplerBuilder() {
        return new SamplerBuilder(this);
    }
    ShaderModuleBuilder* DeviceBase::CreateShaderModuleBuilder() {
        return new ShaderModuleBuilder(this);
    }
    TextureBuilder* DeviceBase::CreateTextureBuilder() {
        return new TextureBuilder(this);
    }

    void DeviceBase::CopyBindGroups(uint32_t start, uint32_t count, BindGroupBase* source, BindGroupBase* target) {
        // TODO(cwallez@chromium.org): update state tracking then call the backend
    }

}
