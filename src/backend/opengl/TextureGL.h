// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_OPENGL_TEXTUREGL_H_
#define BACKEND_OPENGL_TEXTUREGL_H_

#include "common/Texture.h"

#include "glad/glad.h"

namespace backend {
namespace opengl {

    class Device;

    struct TextureFormatInfo {
        GLenum internalFormat;
        GLenum format;
        GLenum type;
    };

    class Texture : public TextureBase {
        public:
            Texture(Device* device, TextureBuilder* builder);

            GLuint GetHandle() const;
            GLenum GetGLTarget() const;
            TextureFormatInfo GetGLFormat() const;

        private:
            Device* device;
            GLuint handle;
            GLenum target;
    };

    class TextureView : public TextureViewBase {
        public:
            TextureView(Device* device, TextureViewBuilder* builder);

        private:
            Device* device;
    };


}
}

#endif // BACKEND_OPENGL_TEXTUREGL_H_
