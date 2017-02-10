// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_COMMON_PIPELINELAYOUT_H_
#define BACKEND_COMMON_PIPELINELAYOUT_H_

#include "Forward.h"
#include "RefCounted.h"

#include "nxt/nxtcpp.h"

#include <array>
#include <bitset>

namespace backend {

    using BindGroupLayoutArray = std::array<Ref<BindGroupLayoutBase>, kMaxBindGroups>;

    class PipelineLayoutBase : public RefCounted {
        public:
            PipelineLayoutBase(PipelineLayoutBuilder* builder);

            const BindGroupLayoutBase* GetBindGroupLayout(size_t group) const;
            const std::bitset<kMaxBindGroups> GetBindGroupsLayoutMask() const;

        protected:
            BindGroupLayoutArray bindGroupLayouts;
            std::bitset<kMaxBindGroups> mask;
    };

    class PipelineLayoutBuilder : public RefCounted {
        public:
            PipelineLayoutBuilder(DeviceBase* device);

            bool WasConsumed() const;

            // NXT API
            PipelineLayoutBase* GetResult();
            void SetBindGroupLayout(uint32_t groupIndex, BindGroupLayoutBase* layout);

        private:
            friend class PipelineLayoutBase;

            DeviceBase* device;
            BindGroupLayoutArray bindGroupLayouts;
            std::bitset<kMaxBindGroups> mask;
            bool consumed = false;
    };

}

#endif // BACKEND_COMMON_PIPELINELAYOUT_H_
