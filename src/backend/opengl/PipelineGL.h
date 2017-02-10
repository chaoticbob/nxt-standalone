// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_OPENGL_PIPELINEGL_H_
#define BACKEND_OPENGL_PIPELINEGL_H_

#include "common/Pipeline.h"

#include "glad/glad.h"

#include <vector>

namespace backend {
namespace opengl {

    class Device;
    class ShaderModule;

    class Pipeline : public PipelineBase {
        public:
            Pipeline(Device* device, PipelineBuilder* builder);

            using GLPushConstantInfo = std::array<GLint, kMaxPushConstants>;
            using BindingLocations = std::array<std::array<GLint, kMaxBindingsPerGroup>, kMaxBindGroups>;

            const GLPushConstantInfo& GetGLPushConstants(nxt::ShaderStage stage) const;
            const std::vector<GLuint>& GetTextureUnitsForSampler(GLuint index) const;
            const std::vector<GLuint>& GetTextureUnitsForTexture(GLuint index) const;
            GLuint GetProgramHandle() const;

            void ApplyNow();

        private:
            GLuint program;
            PerStage<GLPushConstantInfo> glPushConstants;
            std::vector<std::vector<GLuint>> unitsForSamplers;
            std::vector<std::vector<GLuint>> unitsForTextures;
            Device* device;
    };

}
}

#endif // BACKEND_OPENGL_PIPELINEGL_H_
