// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_COMMON_PIPELINE_H_
#define BACKEND_COMMON_PIPELINE_H_

#include "Forward.h"
#include "PerStage.h"
#include "RefCounted.h"

#include "nxt/nxtcpp.h"

#include <array>
#include <bitset>

namespace backend {

    enum PushConstantType : uint8_t {
        Int,
        UInt,
        Float,
    };

    class PipelineBase : public RefCounted {
        public:
            PipelineBase(PipelineBuilder* builder);

            struct PushConstantInfo {
                std::bitset<kMaxPushConstants> mask;
                std::array<PushConstantType, kMaxPushConstants> types;
            };
            const PushConstantInfo& GetPushConstants(nxt::ShaderStage stage) const;
            nxt::ShaderStageBit GetStageMask() const;

            PipelineLayoutBase* GetLayout();
            InputStateBase* GetInputState();

            // TODO(cwallez@chromium.org): split compute and render pipelines
            bool IsCompute() const;

        private:
            DeviceBase* device;

            nxt::ShaderStageBit stageMask;
            Ref<PipelineLayoutBase> layout;
            PerStage<PushConstantInfo> pushConstants;
            Ref<InputStateBase> inputState;
    };

    class PipelineBuilder : public RefCounted {
        public:
            PipelineBuilder(DeviceBase* device);

            bool WasConsumed() const;

            struct StageInfo {
                std::string entryPoint;
                Ref<ShaderModuleBase> module;
            };
            const StageInfo& GetStageInfo(nxt::ShaderStage stage) const;

            // NXT API
            PipelineBase* GetResult();
            void SetLayout(PipelineLayoutBase* layout);
            void SetStage(nxt::ShaderStage stage, ShaderModuleBase* module, const char* entryPoint);
            void SetInputState(InputStateBase* inputState);

        private:
            friend class PipelineBase;

            DeviceBase* device;
            Ref<PipelineLayoutBase> layout;
            nxt::ShaderStageBit stageMask;
            PerStage<StageInfo> stages;
            Ref<InputStateBase> inputState;
            bool consumed = false;
    };

}

#endif // BACKEND_COMMON_PIPELINE_H_
