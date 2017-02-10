// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_COMMON_TEXTURE_H_
#define BACKEND_COMMON_TEXTURE_H_

#include "Forward.h"
#include "RefCounted.h"

#include "nxt/nxtcpp.h"

namespace backend {

    size_t TextureFormatPixelSize(nxt::TextureFormat format);

    class TextureBase : public RefCounted {
        public:
            TextureBase(TextureBuilder* builder);

            nxt::TextureDimension GetDimension() const;
            nxt::TextureFormat GetFormat() const;
            uint32_t GetWidth() const;
            uint32_t GetHeight() const;
            uint32_t GetDepth() const;
            uint32_t GetNumMipLevels() const;
            nxt::TextureUsageBit GetUsage() const;

            // NXT API
            TextureViewBuilder* CreateTextureViewBuilder();

        private:
            DeviceBase* device;

            nxt::TextureDimension dimension;
            nxt::TextureFormat format;
            uint32_t width, height, depth;
            uint32_t numMipLevels;
            nxt::TextureUsageBit usage;
    };

    class TextureBuilder : public RefCounted {
        public:
            TextureBuilder(DeviceBase* device);

            bool WasConsumed() const;

            // NXT API
            TextureBase* GetResult();
            void SetDimension(nxt::TextureDimension dimension);
            void SetExtent(uint32_t width, uint32_t height, uint32_t depth);
            void SetFormat(nxt::TextureFormat format);
            void SetMipLevels(uint32_t numMipLevels);
            void SetUsage(nxt::TextureUsageBit usage);

        private:
            friend class TextureBase;

            DeviceBase* device;
            int propertiesSet = 0;
            bool consumed = false;

            nxt::TextureDimension dimension;
            uint32_t width, height, depth;
            nxt::TextureFormat format;
            uint32_t numMipLevels;
            nxt::TextureUsageBit usage;
    };

    class TextureViewBase : public RefCounted {
        public:
            TextureViewBase(TextureViewBuilder* builder);

            TextureBase* GetTexture();

        private:
            Ref<TextureBase> texture;
    };

    class TextureViewBuilder : public RefCounted {
        public:
            TextureViewBuilder(DeviceBase* device, TextureBase* texture);

            bool WasConsumed() const;

            // NXT API
            TextureViewBase* GetResult();

        private:
            friend class TextureViewBase;

            DeviceBase* device;
            bool consumed = false;
            Ref<TextureBase> texture;
    };

}

#endif // BACKEND_COMMON_TEXTURE_H_
