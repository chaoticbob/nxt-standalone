// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_COMMON_SHADERMODULE_H_
#define BACKEND_COMMON_SHADERMODULE_H_

#include "Forward.h"
#include "RefCounted.h"

#include "nxt/nxtcpp.h"

#include <array>
#include <bitset>
#include <vector>

namespace spirv_cross {
    class Compiler;
}

namespace backend {

    class ShaderModuleBase : public RefCounted {
        public:
            ShaderModuleBase(ShaderModuleBuilder* builder);

            void ExtractSpirvInfo(const spirv_cross::Compiler& compiler);

            struct PushConstantInfo {
                std::bitset<kMaxPushConstants> mask;

                std::array<std::string, kMaxPushConstants> names;
                std::array<int, kMaxPushConstants> sizes;
                std::array<PushConstantType, kMaxPushConstants> types;
            };

            struct BindingInfo {
                // The SPIRV ID of the resource.
                uint32_t id;
                uint32_t base_type_id;
                nxt::BindingType type;
                bool used = false;
            };
            using ModuleBindingInfo = std::array<std::array<BindingInfo, kMaxBindingsPerGroup>, kMaxBindGroups>;

            const PushConstantInfo& GetPushConstants() const;
            const ModuleBindingInfo& GetBindingInfo() const;
            const std::bitset<kMaxVertexAttributes>& GetUsedVertexAttributes() const;
            nxt::ShaderStage GetExecutionModel() const;

            bool IsCompatibleWithPipelineLayout(const PipelineLayoutBase* layout);

        private:
            bool IsCompatibleWithBindGroupLayout(size_t group, const BindGroupLayoutBase* layout);

            DeviceBase* device;
            PushConstantInfo pushConstants = {};
            ModuleBindingInfo bindingInfo;
            std::bitset<kMaxVertexAttributes> usedVertexAttributes;
            nxt::ShaderStage executionModel;
    };

    class ShaderModuleBuilder : public RefCounted {
        public:
            ShaderModuleBuilder(DeviceBase* device);

            bool WasConsumed() const;

            std::vector<uint32_t> AcquireSpirv();

            // NXT API
            ShaderModuleBase* GetResult();
            void SetSource(uint32_t codeSize, const uint32_t* code);

        private:
            friend class ShaderModuleBase;

            DeviceBase* device;
            std::vector<uint32_t> spirv;
            bool consumed = false;
    };

}

#endif // BACKEND_COMMON_SHADERMODULE_H_
