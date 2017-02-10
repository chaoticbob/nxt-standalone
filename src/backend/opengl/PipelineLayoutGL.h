// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_OPENGL_PIPELINELAYOUTGL_H_
#define BACKEND_OPENGL_PIPELINELAYOUTGL_H_

#include "common/PipelineLayout.h"

#include "glad/glad.h"

namespace backend {
namespace opengl {

    class Device;

    class PipelineLayout : public PipelineLayoutBase {
        public:
            PipelineLayout(Device* device, PipelineLayoutBuilder* builder);

            using BindingIndexInfo = std::array<std::array<GLuint, kMaxBindingsPerGroup>, kMaxBindGroups>;
            const BindingIndexInfo& GetBindingIndexInfo() const;

            GLuint GetTextureUnitsUsed() const;
            size_t GetNumSamplers() const;
            size_t GetNumSampledTextures() const;

        private:
            Device* device;
            BindingIndexInfo indexInfo;
            size_t numSamplers;
            size_t numSampledTextures;
    };

}
}

#endif // BACKEND_OPENGL_PIPELINELAYOUTGL_H_
