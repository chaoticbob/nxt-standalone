// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "TextureGL.h"

#include <algorithm>
#include <vector>

namespace backend {
namespace opengl {

    namespace {

        GLenum TargetForDimension(nxt::TextureDimension dimension) {
            switch (dimension) {
                case nxt::TextureDimension::e2D:
                    return GL_TEXTURE_2D;
            }
        }

        TextureFormatInfo GetGLFormatInfo(nxt::TextureFormat format) {
            switch (format) {
                case nxt::TextureFormat::R8G8B8A8Unorm:
                    return {GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE};
            }
        }

    }

    // Texture

    Texture::Texture(Device* device, TextureBuilder* builder)
        : TextureBase(builder), device(device) {
        target = TargetForDimension(GetDimension());

        uint32_t width = GetWidth();
        uint32_t height = GetHeight();
        uint32_t levels = GetNumMipLevels();

        auto formatInfo = GetGLFormatInfo(GetFormat());

        glGenTextures(1, &handle);
        glBindTexture(target, handle);

        for (uint32_t i = 0; i < levels; ++i) {
            glTexImage2D(target, i, formatInfo.internalFormat, width, height, 0, formatInfo.format, formatInfo.type, nullptr);
            width = std::max(uint32_t(1), width / 2);
            height = std::max(uint32_t(1), height / 2);
        }

        // The texture is not complete if it uses mipmapping and not all levels up to
        // MAX_LEVEL have been defined.
        glTexParameteri(target, GL_TEXTURE_MAX_LEVEL, levels - 1);
    }

    GLuint Texture::GetHandle() const {
        return handle;
    }

    GLenum Texture::GetGLTarget() const {
        return target;
    }

    TextureFormatInfo Texture::GetGLFormat() const {
        return GetGLFormatInfo(GetFormat());
    }

    // TextureView

    TextureView::TextureView(Device* device, TextureViewBuilder* builder)
        : TextureViewBase(builder), device(device) {
    }

}
}
