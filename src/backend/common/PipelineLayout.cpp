// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "PipelineLayout.h"

#include "BindGroupLayout.h"
#include "Device.h"

namespace backend {

    // PipelineLayoutBase

    PipelineLayoutBase::PipelineLayoutBase(PipelineLayoutBuilder* builder)
        : bindGroupLayouts(std::move(builder->bindGroupLayouts)), mask(builder->mask) {
    }

    const BindGroupLayoutBase* PipelineLayoutBase::GetBindGroupLayout(size_t group) const {
        ASSERT(group < kMaxBindGroups);
        return bindGroupLayouts[group].Get();
    }

    const std::bitset<kMaxBindGroups> PipelineLayoutBase::GetBindGroupsLayoutMask() const {
        return mask;
    }

    // PipelineLayoutBuilder

    PipelineLayoutBuilder::PipelineLayoutBuilder(DeviceBase* device) : device(device) {
    }

    bool PipelineLayoutBuilder::WasConsumed() const {
        return consumed;
    }

    PipelineLayoutBase* PipelineLayoutBuilder::GetResult() {
        // TODO(cwallez@chromium.org): this is a hack, have the null bind group layout somewhere in the device
        // once we have a cache of BGL
        for (size_t group = 0; group < kMaxBindGroups; ++group) {
            if (!bindGroupLayouts[group]) {
                bindGroupLayouts[group] = device->CreateBindGroupLayoutBuilder()->GetResult();
            }
        }

        consumed = true;
        return device->CreatePipelineLayout(this);
    }

    void PipelineLayoutBuilder::SetBindGroupLayout(uint32_t groupIndex, BindGroupLayoutBase* layout) {
        if (groupIndex >= kMaxBindGroups) {
            device->HandleError("groupIndex is over the maximum allowed");
            return;
        }
        if (mask[groupIndex]) {
            device->HandleError("Bind group layout already specified");
            return;
        }

        bindGroupLayouts[groupIndex] = layout;
        mask.set(groupIndex);
    }

}
