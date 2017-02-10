// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_OPENGL_SHADERMODULEGL_H_
#define BACKEND_OPENGL_SHADERMODULEGL_H_

#include "common/ShaderModule.h"

#include "glad/glad.h"

namespace backend {
namespace opengl {

    class Device;

    std::string GetBindingName(uint32_t group, uint32_t binding);

    struct BindingLocation {
        uint32_t group;
        uint32_t binding;
    };
    bool operator < (const BindingLocation& a, const BindingLocation& b);

    struct CombinedSampler {
        BindingLocation samplerLocation;
        BindingLocation textureLocation;
        std::string GetName() const;
    };
    bool operator < (const CombinedSampler& a, const CombinedSampler& b);

    class ShaderModule : public ShaderModuleBase {
        public:
            ShaderModule(Device* device, ShaderModuleBuilder* builder);

            using CombinedSamplerInfo = std::vector<CombinedSampler>;

            const char* GetSource() const;
            const CombinedSamplerInfo& GetCombinedSamplerInfo() const;

        private:
            Device* device;
            CombinedSamplerInfo combinedInfo;
            std::string glslSource;
    };

}
}

#endif // BACKEND_OPENGL_SHADERMODULEGL_H_
