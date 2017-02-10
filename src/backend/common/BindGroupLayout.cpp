// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "BindGroupLayout.h"

#include "Device.h"

#include <functional>

namespace backend {

    namespace {

        // Workaround for Chrome's stdlib having a broken std::hash for enums and bitsets
        template<typename T>
        typename std::enable_if<std::is_enum<T>::value, size_t>::type Hash(T value) {
            using Integral = typename nxt::UnderlyingType<T>::type;
            return std::hash<Integral>()(static_cast<Integral>(value));
        }

        template<size_t N>
        size_t Hash(const std::bitset<N>& value) {
            static_assert(N <= sizeof(unsigned long long) * 8, "");
            return std::hash<unsigned long long>()(value.to_ullong());
        }


        // TODO(cwallez@chromium.org): see if we can use boost's hash combined or some equivalent
        // this currently assumes that size_t is 64 bits
        void CombineHashes(size_t* h1, size_t h2) {
            *h1 ^= (h2 << 7) + (h2 >> (64 - 7)) + 0x304975;
        }

        size_t HashBindingInfo(const BindGroupLayoutBase::LayoutBindingInfo& info) {
            size_t hash = Hash(info.mask);

            for (size_t binding = 0; binding < kMaxBindingsPerGroup; ++binding) {
                if (info.mask[binding]) {
                    CombineHashes(&hash, Hash(info.visibilities[binding]));
                    CombineHashes(&hash, Hash(info.types[binding]));
                }
            }

            return hash;
        }

        bool operator== (const BindGroupLayoutBase::LayoutBindingInfo& a, const BindGroupLayoutBase::LayoutBindingInfo& b) {
            if (a.mask != b.mask) {
                return false;
            }

            for (size_t binding = 0; binding < kMaxBindingsPerGroup; ++binding) {
                if (a.mask[binding]) {
                    if (a.visibilities[binding] != b.visibilities[binding]) {
                        return false;
                    }
                    if (a.types[binding] != b.types[binding]) {
                        return false;
                    }
                }
            }

            return true;
        }
    }

    // BindGroupLayoutBase

    BindGroupLayoutBase::BindGroupLayoutBase(BindGroupLayoutBuilder* builder, bool blueprint)
        : device(builder->device), bindingInfo(builder->bindingInfo), blueprint(blueprint) {
    }

    BindGroupLayoutBase::~BindGroupLayoutBase() {
        // Do not register the actual cached object if we are a blueprint
        if (!blueprint) {
            device->UncacheBindGroupLayout(this);
        }
    }

    const BindGroupLayoutBase::LayoutBindingInfo& BindGroupLayoutBase::GetBindingInfo() const {
        return bindingInfo;
    }

    // BindGroupLayoutBuilder

    BindGroupLayoutBuilder::BindGroupLayoutBuilder(DeviceBase* device) : device(device) {
    }

    bool BindGroupLayoutBuilder::WasConsumed() const {
        return consumed;
    }

    const BindGroupLayoutBase::LayoutBindingInfo& BindGroupLayoutBuilder::GetBindingInfo() const {
        return bindingInfo;
    }

    BindGroupLayoutBase* BindGroupLayoutBuilder::GetResult() {
        consumed = true;
        BindGroupLayoutBase blueprint(this, true);

        auto* result = device->GetOrCreateBindGroupLayout(&blueprint, this);
        result->Reference();
        return result;
    }

    void BindGroupLayoutBuilder::SetBindingsType(nxt::ShaderStageBit visibility, nxt::BindingType bindingType, uint32_t start, uint32_t count) {
        if (start + count > kMaxBindingsPerGroup) {
            device->HandleError("Setting bindings type over maximum number of bindings");
            return;
        }

        for (size_t i = start; i < start + count; i++) {
            if (bindingInfo.mask[i]) {
                device->HandleError("Setting already set binding type");
                return;
            }
            bindingInfo.mask.set(i);
            bindingInfo.visibilities[i] = visibility;
            bindingInfo.types[i] = bindingType;
        }
    }

    // BindGroupLayoutCacheFuncs

    size_t BindGroupLayoutCacheFuncs::operator() (const BindGroupLayoutBase* bgl) const {
        return HashBindingInfo(bgl->GetBindingInfo());
    }

    bool BindGroupLayoutCacheFuncs::operator() (const BindGroupLayoutBase* a, const BindGroupLayoutBase* b) const {
        return a->GetBindingInfo() == b->GetBindingInfo();
    }

}
