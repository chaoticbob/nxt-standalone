// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_OPENGL_SAMPLERGL_H_
#define BACKEND_OPENGL_SAMPLERGL_H_

#include "common/Sampler.h"

#include "glad/glad.h"

namespace backend {
namespace opengl {

    class Device;

    class Sampler : public SamplerBase {
        public:
            Sampler(Device* device, SamplerBuilder* builder);

            GLuint GetHandle() const;

        private:
            Device* device;
            GLuint handle;
    };

}
}

#endif // BACKEND_OPENGL_SAMPLERGL_H_
