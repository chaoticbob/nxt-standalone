// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "Buffer.h"

#include "Device.h"

#include <utility>

namespace backend {

    // Buffer

    BufferBase::BufferBase(BufferBuilder* builder)
        : device(builder->device), size(builder->size), usage(builder->usage) {
    }

    BufferViewBuilder* BufferBase::CreateBufferViewBuilder() {
        return new BufferViewBuilder(device, this);
    }

    uint32_t BufferBase::GetSize() const {
        return size;
    }

    nxt::BufferUsageBit BufferBase::GetUsage() const {
        return usage;
    }

    void BufferBase::SetSubData(uint32_t start, uint32_t count, const uint32_t* data) {
        if ((start + count) * sizeof(uint32_t) > GetSize()) {
            device->HandleError("Buffer subdata out of range");
            return;
        }

        if (!(usage & nxt::BufferUsageBit::Mapped)) {
            device->HandleError("Buffer needs the mapped usage bit");
            return;
        }

        SetSubDataImpl(start, count, data);
    }

    // BufferBuilder

    enum BufferSetProperties {
        BUFFER_PROPERTY_USAGE = 0x1,
        BUFFER_PROPERTY_SIZE = 0x2,
    };

    BufferBuilder::BufferBuilder(DeviceBase* device) : device(device) {
    }

    bool BufferBuilder::WasConsumed() const {
        return consumed;
    }

    BufferBase* BufferBuilder::GetResult() {
        constexpr int allProperties = BUFFER_PROPERTY_USAGE | BUFFER_PROPERTY_SIZE;
        if ((propertiesSet & allProperties) != allProperties) {
            device->HandleError("Buffer missing properties");
            return nullptr;
        }

        consumed = true;
        return device->CreateBuffer(this);
    }

    void BufferBuilder::SetUsage(nxt::BufferUsageBit usage) {
        if ((propertiesSet & BUFFER_PROPERTY_USAGE) != 0) {
            device->HandleError("Buffer usage property set multiple times");
            return;
        }

        this->usage = usage;
        propertiesSet |= BUFFER_PROPERTY_USAGE;
    }

    void BufferBuilder::SetSize(uint32_t size) {
        if ((propertiesSet & BUFFER_PROPERTY_SIZE) != 0) {
            device->HandleError("Buffer size property set multiple times");
            return;
        }

        this->size = size;
        propertiesSet |= BUFFER_PROPERTY_SIZE;
    }

    // BufferViewBase

    BufferViewBase::BufferViewBase(BufferViewBuilder* builder)
        : buffer(std::move(builder->buffer)), size(builder->size), offset(builder->offset) {
    }

    BufferBase* BufferViewBase::GetBuffer() {
        return buffer.Get();
    }

    uint32_t BufferViewBase::GetSize() const {
        return size;
    }

    uint32_t BufferViewBase::GetOffset() const {
        return offset;
    }

    // BufferViewBuilder

    enum BufferViewSetProperties {
        BUFFER_VIEW_PROPERTY_EXTENT = 0x1,
    };

    BufferViewBuilder::BufferViewBuilder(DeviceBase* device, BufferBase* buffer)
        : device(device), buffer(buffer) {
    }

    bool BufferViewBuilder::WasConsumed() const {
        return consumed;
    }

    BufferViewBase* BufferViewBuilder::GetResult() {
        constexpr int allProperties = BUFFER_VIEW_PROPERTY_EXTENT;
        if ((propertiesSet & allProperties) != allProperties) {
            device->HandleError("Buffer view missing properties");
            return nullptr;
        }

        return device->CreateBufferView(this);
    }

    void BufferViewBuilder::SetExtent(uint32_t offset, uint32_t size) {
        if ((propertiesSet & BUFFER_VIEW_PROPERTY_EXTENT) != 0) {
            device->HandleError("Buffer view extent property set multiple times");
            return;
        }

        uint64_t viewEnd = static_cast<uint64_t>(offset) + static_cast<uint64_t>(size);
        if (viewEnd > static_cast<uint64_t>(buffer->GetSize())) {
            device->HandleError("Buffer view end is OOB");
            return;
        }

        this->offset = offset;
        this->size = size;
        propertiesSet |= BUFFER_VIEW_PROPERTY_EXTENT;
    }

}
