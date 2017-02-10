// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "SamplerGL.h"

namespace backend {
namespace opengl {

    namespace {
        GLenum MagFilterMode(nxt::FilterMode filter) {
            switch (filter) {
                case nxt::FilterMode::Nearest:
                    return GL_NEAREST;
                case nxt::FilterMode::Linear:
                    return GL_LINEAR;
            }
        }

        GLenum MinFilterMode(nxt::FilterMode minFilter, nxt::FilterMode mipMapFilter) {
            switch (minFilter) {
                case nxt::FilterMode::Nearest:
                    switch (mipMapFilter) {
                        case nxt::FilterMode::Nearest:
                            return GL_NEAREST_MIPMAP_NEAREST;
                        case nxt::FilterMode::Linear:
                            return GL_NEAREST_MIPMAP_LINEAR;
                    }
                case nxt::FilterMode::Linear:
                    switch (mipMapFilter) {
                        case nxt::FilterMode::Nearest:
                            return GL_LINEAR_MIPMAP_NEAREST;
                        case nxt::FilterMode::Linear:
                            return GL_LINEAR_MIPMAP_LINEAR;
                    }
            }
        }
    }

    Sampler::Sampler(Device* device, SamplerBuilder* builder)
        : SamplerBase(builder), device(device) {
        glGenSamplers(1, &handle);
        glSamplerParameteri(handle, GL_TEXTURE_MAG_FILTER, MagFilterMode(builder->GetMagFilter()));
        glSamplerParameteri(handle, GL_TEXTURE_MIN_FILTER, MinFilterMode(builder->GetMinFilter(), builder->GetMipMapFilter()));
    }

    GLuint Sampler::GetHandle() const {
        return handle;
    }

}
}
