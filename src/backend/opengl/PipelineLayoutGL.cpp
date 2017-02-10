// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "PipelineLayoutGL.h"

#include "OpenGLBackend.h"

namespace backend {
namespace opengl {

    PipelineLayout::PipelineLayout(Device* device, PipelineLayoutBuilder* builder)
        : PipelineLayoutBase(builder), device(device) {
        GLuint uboIndex = 0;
        GLuint samplerIndex = 0;
        GLuint sampledTextureIndex = 0;
        GLuint ssboIndex = 0;

        for (size_t group = 0; group < kMaxBindGroups; ++group) {
            const auto& groupInfo = GetBindGroupLayout(group)->GetBindingInfo();

            for (size_t binding = 0; binding < kMaxBindingsPerGroup; ++binding) {
                if (!groupInfo.mask[binding]) {
                    continue;
                }

                switch (groupInfo.types[binding]) {
                    case nxt::BindingType::UniformBuffer:
                        indexInfo[group][binding] = uboIndex;
                        uboIndex ++;
                        break;
                    case nxt::BindingType::Sampler:
                        indexInfo[group][binding] = samplerIndex;
                        samplerIndex ++;
                        break;
                    case nxt::BindingType::SampledTexture:
                        indexInfo[group][binding] = sampledTextureIndex;
                        sampledTextureIndex ++;
                        break;

                    case nxt::BindingType::StorageBuffer:
                        indexInfo[group][binding] = ssboIndex;
                        ssboIndex ++;
                        break;
                }
            }
        }

        numSamplers = samplerIndex;
        numSampledTextures = sampledTextureIndex;
    }

    const PipelineLayout::BindingIndexInfo& PipelineLayout::GetBindingIndexInfo() const {
        return indexInfo;
    }

    GLuint PipelineLayout::GetTextureUnitsUsed() const {
        return 0;
    }

    size_t PipelineLayout::GetNumSamplers() const {
        return numSamplers;
    }

    size_t PipelineLayout::GetNumSampledTextures() const {
        return numSampledTextures;
    }

}
}
