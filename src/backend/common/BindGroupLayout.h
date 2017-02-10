// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_COMMON_BINDGROUPLAYOUT_H_
#define BACKEND_COMMON_BINDGROUPLAYOUT_H_

#include "Forward.h"
#include "RefCounted.h"

#include "nxt/nxtcpp.h"

#include <array>
#include <bitset>

namespace backend {

    class BindGroupLayoutBase : public RefCounted {
        public:
            BindGroupLayoutBase(BindGroupLayoutBuilder* builder, bool blueprint = false);
            ~BindGroupLayoutBase() override;

            struct LayoutBindingInfo {
                std::array<nxt::ShaderStageBit, kMaxBindingsPerGroup> visibilities;
                std::array<nxt::BindingType, kMaxBindingsPerGroup> types;
                std::bitset<kMaxBindingsPerGroup> mask;
            };
            const LayoutBindingInfo& GetBindingInfo() const;

        private:
            DeviceBase* device;
            LayoutBindingInfo bindingInfo;
            bool blueprint = false;
    };

    class BindGroupLayoutBuilder : public RefCounted {
        public:
            BindGroupLayoutBuilder(DeviceBase* device);

            bool WasConsumed() const;
            const BindGroupLayoutBase::LayoutBindingInfo& GetBindingInfo() const;

            // NXT API
            BindGroupLayoutBase* GetResult();
            void SetBindingsType(nxt::ShaderStageBit visibility, nxt::BindingType bindingType, uint32_t start, uint32_t count);

        private:
            friend class BindGroupLayoutBase;

            DeviceBase* device;
            BindGroupLayoutBase::LayoutBindingInfo bindingInfo;
            bool consumed = false;
    };

    // Implements the functors necessary for the unordered_set<BGL*>-based cache.
    struct BindGroupLayoutCacheFuncs {
        // The hash function
        size_t operator() (const BindGroupLayoutBase* bgl) const;

        // The equality predicate
        bool operator() (const BindGroupLayoutBase* a, const BindGroupLayoutBase* b) const;
    };

}

#endif // BACKEND_COMMON_BINDGROUPLAYOUT_H_
