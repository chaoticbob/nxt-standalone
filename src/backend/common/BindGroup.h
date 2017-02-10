// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_COMMON_BINDGROUP_H_
#define BACKEND_COMMON_BINDGROUP_H_

#include "Forward.h"
#include "RefCounted.h"

#include "nxt/nxtcpp.h"

#include <array>
#include <bitset>
#include <type_traits>

namespace backend {

    class BindGroupBase : public RefCounted {
        public:
            BindGroupBase(BindGroupBuilder* builder);

            const BindGroupLayoutBase* GetLayout() const;
            nxt::BindGroupUsage GetUsage() const;
            BufferViewBase* GetBindingAsBufferView(size_t binding);
            SamplerBase* GetBindingAsSampler(size_t binding);
            TextureViewBase* GetBindingAsTextureView(size_t binding);

        private:
            Ref<BindGroupLayoutBase> layout;
            nxt::BindGroupUsage usage;
            std::array<Ref<RefCounted>, kMaxBindingsPerGroup> bindings;
    };

    class BindGroupBuilder : public RefCounted {
        public:
            BindGroupBuilder(DeviceBase* device);

            bool WasConsumed() const;

            // NXT API
            BindGroupBase* GetResult();
            void SetLayout(BindGroupLayoutBase* layout);
            void SetUsage(nxt::BindGroupUsage usage);

            template<typename T>
            void SetBufferViews(uint32_t start, uint32_t count, T* const* bufferViews) {
                static_assert(std::is_base_of<BufferViewBase, T>::value, "");
                SetBufferViews(start, count, reinterpret_cast<BufferViewBase* const*>(bufferViews));
            }
            void SetBufferViews(uint32_t start, uint32_t count, BufferViewBase* const * bufferViews);

            template<typename T>
            void SetSamplers(uint32_t start, uint32_t count, T* const* samplers) {
                static_assert(std::is_base_of<SamplerBase, T>::value, "");
                SetSamplers(start, count, reinterpret_cast<SamplerBase* const*>(samplers));
            }
            void SetSamplers(uint32_t start, uint32_t count, SamplerBase* const * samplers);

            template<typename T>
            void SetTextureViews(uint32_t start, uint32_t count, T* const* textureViews) {
                static_assert(std::is_base_of<TextureViewBase, T>::value, "");
                SetTextureViews(start, count, reinterpret_cast<TextureViewBase* const*>(textureViews));
            }
            void SetTextureViews(uint32_t start, uint32_t count, TextureViewBase* const * textureViews);

        private:
            friend class BindGroupBase;

            void SetBindingsBase(uint32_t start, uint32_t count, RefCounted* const * objects);
            bool SetBindingsValidationBase(uint32_t start, uint32_t count);

            DeviceBase* device;
            std::bitset<kMaxBindingsPerGroup> setMask;
            int propertiesSet = 0;
            bool consumed = false;

            Ref<BindGroupLayoutBase> layout;
            nxt::BindGroupUsage usage;
            std::array<Ref<RefCounted>, kMaxBindingsPerGroup> bindings;
    };

}

#endif // BACKEND_COMMON_BINDGROUP_H_
