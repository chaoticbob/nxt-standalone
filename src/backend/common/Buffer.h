// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_COMMON_BUFFER_H_
#define BACKEND_COMMON_BUFFER_H_

#include "Forward.h"
#include "RefCounted.h"

#include "nxt/nxtcpp.h"

namespace backend {

    class BufferBase : public RefCounted {
        public:
            BufferBase(BufferBuilder* builder);

            uint32_t GetSize() const;
            nxt::BufferUsageBit GetUsage() const;

            // NXT API
            BufferViewBuilder* CreateBufferViewBuilder();
            void SetSubData(uint32_t start, uint32_t count, const uint32_t* data);

        private:
            virtual void SetSubDataImpl(uint32_t start, uint32_t count, const uint32_t* data) = 0;

            DeviceBase* device;
            uint32_t size;
            nxt::BufferUsageBit usage;
    };

    class BufferBuilder : public RefCounted {
        public:
            BufferBuilder(DeviceBase* device);

            bool WasConsumed() const;

            // NXT API
            BufferBase* GetResult();
            void SetUsage(nxt::BufferUsageBit usage);
            void SetSize(uint32_t size);

        private:
            friend class BufferBase;

            DeviceBase* device;
            uint32_t size;
            nxt::BufferUsageBit usage;
            int propertiesSet = 0;
            bool consumed = false;
    };

    class BufferViewBase : public RefCounted {
        public:
            BufferViewBase(BufferViewBuilder* builder);

            BufferBase* GetBuffer();
            uint32_t GetSize() const;
            uint32_t GetOffset() const;

        private:
            Ref<BufferBase> buffer;
            uint32_t size;
            uint32_t offset;
    };

    class BufferViewBuilder : public RefCounted {
        public:
            BufferViewBuilder(DeviceBase* device, BufferBase* buffer);

            bool WasConsumed() const;

            // NXT API
            BufferViewBase* GetResult();
            void SetExtent(uint32_t offset, uint32_t size);

        private:
            friend class BufferViewBase;

            DeviceBase* device;
            Ref<BufferBase> buffer;
            uint32_t offset = 0;
            uint32_t size = 0;
            int propertiesSet = 0;
            bool consumed = false;
    };

}

#endif // BACKEND_COMMON_BUFFER_H_
