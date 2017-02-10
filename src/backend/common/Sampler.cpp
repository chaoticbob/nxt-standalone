// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "Sampler.h"

#include "Device.h"

namespace backend {

    // SamplerBase

    SamplerBase::SamplerBase(SamplerBuilder* builder) {
    }

    // SamplerBuilder

    enum SamplerSetProperties {
        SAMPLER_PROPERTY_FILTER = 0x1,
    };
    SamplerBuilder::SamplerBuilder(DeviceBase* device)
        :device(device) {
    }

    nxt::FilterMode SamplerBuilder::GetMagFilter() const {
        return magFilter;
    }

    nxt::FilterMode SamplerBuilder::GetMinFilter() const {
        return minFilter;
    }

    nxt::FilterMode SamplerBuilder::GetMipMapFilter() const {
        return mipMapFilter;
    }

    bool SamplerBuilder::WasConsumed() const {
        return consumed;
    }

    SamplerBase* SamplerBuilder::GetResult() {
        consumed = true;
        return device->CreateSampler(this);
    }

    void SamplerBuilder::SetFilterMode(nxt::FilterMode magFilter, nxt::FilterMode minFilter, nxt::FilterMode mipMapFilter) {
        if ((propertiesSet & SAMPLER_PROPERTY_FILTER) != 0) {
            device->HandleError("Sampler filter property set multiple times");
            return;
        }

        this->magFilter = magFilter;
        this->minFilter = minFilter;
        this->mipMapFilter = mipMapFilter;
        propertiesSet |= SAMPLER_PROPERTY_FILTER;
    }
}
