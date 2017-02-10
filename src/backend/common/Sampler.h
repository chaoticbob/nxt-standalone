// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_COMMON_SAMPLER_H_
#define BACKEND_COMMON_SAMPLER_H_

#include "Forward.h"
#include "RefCounted.h"

#include "nxt/nxtcpp.h"

namespace backend {

    class SamplerBase : public RefCounted {
        public:
            SamplerBase(SamplerBuilder* builder);
    };

    class SamplerBuilder : public RefCounted {
        public:
            SamplerBuilder(DeviceBase* device);

            nxt::FilterMode GetMagFilter() const;
            nxt::FilterMode GetMinFilter() const;
            nxt::FilterMode GetMipMapFilter() const;

            bool WasConsumed() const;

            // NXT API
            SamplerBase* GetResult();
            void SetFilterMode(nxt::FilterMode magFilter, nxt::FilterMode minFilter, nxt::FilterMode mipMapFilter);

        private:
            friend class SamplerBase;

            DeviceBase* device;
            int propertiesSet = 0;
            bool consumed = false;

            nxt::FilterMode magFilter = nxt::FilterMode::Nearest;
            nxt::FilterMode minFilter = nxt::FilterMode::Nearest;
            nxt::FilterMode mipMapFilter = nxt::FilterMode::Nearest;
    };

}

#endif // BACKEND_COMMON_SAMPLER_H_
