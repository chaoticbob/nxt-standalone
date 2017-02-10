// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_COMMON_COMMANDBUFFERGL_H_
#define BACKEND_COMMON_COMMANDBUFFERGL_H_

#include "nxt/nxtcpp.h"

#include "CommandAllocator.h"
#include "RefCounted.h"

namespace backend {

    class BindGroupBase;
    class BufferBase;
    class DeviceBase;
    class PipelineBase;
    class TextureBase;

    class CommandBufferBase : public RefCounted {
    };

    class CommandBufferBuilder : public RefCounted {
        public:
            CommandBufferBuilder(DeviceBase* device);
            ~CommandBufferBuilder();

            bool WasConsumed() const;
            bool ValidateGetResult();

            CommandIterator AcquireCommands();

            // NXT API
            CommandBufferBase* GetResult();

            void CopyBufferToTexture(BufferBase* buffer, TextureBase* texture, uint32_t x, uint32_t y, uint32_t z,
                                     uint32_t width, uint32_t height, uint32_t depth, uint32_t level);
            void Dispatch(uint32_t x, uint32_t y, uint32_t z);
            void DrawArrays(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);
            void DrawElements(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstIndex, uint32_t firstInstance);
            void SetPushConstants(nxt::ShaderStageBit stage, uint32_t offset, uint32_t count, const void* data);
            void SetPipeline(PipelineBase* pipeline);
            void SetBindGroup(uint32_t groupIndex, BindGroupBase* group);
            void SetIndexBuffer(BufferBase* buffer, uint32_t offset, nxt::IndexFormat format);

            template<typename T>
            void SetVertexBuffers(uint32_t startSlot, uint32_t count, T* const* buffers, uint32_t const* offsets) {
                static_assert(std::is_base_of<BufferBase, T>::value, "");
                SetVertexBuffers(startSlot, count, reinterpret_cast<BufferBase* const*>(buffers), offsets);
            }
            void SetVertexBuffers(uint32_t startSlot, uint32_t count, BufferBase* const* buffers, uint32_t const* offsets);

        private:
            void MoveToIterator();

            DeviceBase* device;
            CommandAllocator allocator;
            CommandIterator iterator;
            bool consumed = false;
            bool movedToIterator = false;
    };

}

#endif // BACKEND_COMMON_COMMANDBUFFERGL_H_
