// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "Texture.h"

#include "Device.h"

namespace backend {

    size_t TextureFormatPixelSize(nxt::TextureFormat format) {
        switch (format) {
            case nxt::TextureFormat::R8G8B8A8Unorm:
                return 4;
        }
    }

    // TextureBase

    TextureBase::TextureBase(TextureBuilder* builder)
        : device(builder->device), dimension(builder->dimension), format(builder->format), width(builder->width),
        height(builder->height), depth(builder->depth), numMipLevels(builder->numMipLevels), usage(builder->usage) {
    }

    nxt::TextureDimension TextureBase::GetDimension() const {
        return dimension;
    }
    nxt::TextureFormat TextureBase::GetFormat() const {
        return format;
    }
    uint32_t TextureBase::GetWidth() const {
        return width;
    }
    uint32_t TextureBase::GetHeight() const {
        return height;
    }
    uint32_t TextureBase::GetDepth() const {
        return depth;
    }
    uint32_t TextureBase::GetNumMipLevels() const {
        return numMipLevels;
    }
    nxt::TextureUsageBit TextureBase::GetUsage() const {
        return usage;
    }

    TextureViewBuilder* TextureBase::CreateTextureViewBuilder() {
        return new TextureViewBuilder(device, this);
    }

    // TextureBuilder

    enum TextureSetProperties {
        TEXTURE_PROPERTY_DIMENSION = 0x1,
        TEXTURE_PROPERTY_EXTENT = 0x2,
        TEXTURE_PROPERTY_FORMAT = 0x4,
        TEXTURE_PROPERTY_MIP_LEVELS = 0x8,
        TEXTURE_PROPERTY_USAGE = 0x10,
    };

    TextureBuilder::TextureBuilder(DeviceBase* device)
        : device(device) {
    }

    bool TextureBuilder::WasConsumed() const {
        return consumed;
    }

    TextureBase* TextureBuilder::GetResult() {
        constexpr int allProperties = TEXTURE_PROPERTY_DIMENSION | TEXTURE_PROPERTY_EXTENT |
            TEXTURE_PROPERTY_FORMAT | TEXTURE_PROPERTY_MIP_LEVELS | TEXTURE_PROPERTY_USAGE;
        if ((propertiesSet & allProperties) != allProperties) {
            device->HandleError("Texture missing properties");
            return nullptr;
        }

        // TODO(cwallez@chromium.org): check stuff based on the dimension

        consumed = true;
        return device->CreateTexture(this);
    }

    void TextureBuilder::SetDimension(nxt::TextureDimension dimension) {
        if ((propertiesSet & TEXTURE_PROPERTY_DIMENSION) != 0) {
            device->HandleError("Texture dimension property set multiple times");
            return;
        }

        propertiesSet |= TEXTURE_PROPERTY_DIMENSION;
        this->dimension = dimension;
    }

    void TextureBuilder::SetExtent(uint32_t width, uint32_t height, uint32_t depth) {
        if ((propertiesSet & TEXTURE_PROPERTY_EXTENT) != 0) {
            device->HandleError("Texture extent property set multiple times");
            return;
        }

        if (width == 0 || height == 0 || depth == 0) {
            device->HandleError("Cannot create an empty texture");
            return;
        }

        propertiesSet |= TEXTURE_PROPERTY_EXTENT;
        this->width = width;
        this->height = height;
        this->depth = depth;
    }

    void TextureBuilder::SetFormat(nxt::TextureFormat format) {
        if ((propertiesSet & TEXTURE_PROPERTY_FORMAT) != 0) {
            device->HandleError("Texture format property set multiple times");
            return;
        }

        propertiesSet |= TEXTURE_PROPERTY_FORMAT;
        this->format = format;
    }

    void TextureBuilder::SetMipLevels(uint32_t numMipLevels) {
        if ((propertiesSet & TEXTURE_PROPERTY_MIP_LEVELS) != 0) {
            device->HandleError("Texture mip levels property set multiple times");
            return;
        }

        propertiesSet |= TEXTURE_PROPERTY_MIP_LEVELS;
        this->numMipLevels = numMipLevels;
    }

    void TextureBuilder::SetUsage(nxt::TextureUsageBit usage) {
        if ((propertiesSet & TEXTURE_PROPERTY_USAGE) != 0) {
            device->HandleError("Texture usage property set multiple times");
            return;
        }

        propertiesSet |= TEXTURE_PROPERTY_USAGE;
        this->usage = usage;
    }

    // TextureViewBase

    TextureViewBase::TextureViewBase(TextureViewBuilder* builder)
        : texture(builder->texture) {
    }

    TextureBase* TextureViewBase::GetTexture() {
        return texture.Get();
    }

    // TextureViewBuilder

    TextureViewBuilder::TextureViewBuilder(DeviceBase* device, TextureBase* texture)
        : device(device), texture(texture) {
    }

    bool TextureViewBuilder::WasConsumed() const {
        return false;
    }

    TextureViewBase* TextureViewBuilder::GetResult() {
        consumed = true;
        return device->CreateTextureView(this);
    }

}
