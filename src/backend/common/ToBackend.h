// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_COMMON_TOBACKEND_H_
#define BACKEND_COMMON_TOBACKEND_H_

#include "Forward.h"

namespace backend {

    // ToBackendTraits implements the mapping from base type to member type of BackendTraits
    template<typename T, typename BackendTraits>
    struct ToBackendTraits;

    template<typename BackendTraits>
    struct ToBackendTraits<BindGroupBase, BackendTraits> {
        using BackendType = typename BackendTraits::BindGroupType;
    };

    template<typename BackendTraits>
    struct ToBackendTraits<BindGroupLayoutBase, BackendTraits> {
        using BackendType = typename BackendTraits::BindGroupLayoutType;
    };

    template<typename BackendTraits>
    struct ToBackendTraits<BufferBase, BackendTraits> {
        using BackendType = typename BackendTraits::BufferType;
    };

    template<typename BackendTraits>
    struct ToBackendTraits<BufferViewBase, BackendTraits> {
        using BackendType = typename BackendTraits::BufferViewType;
    };

    template<typename BackendTraits>
    struct ToBackendTraits<CommandBufferBase, BackendTraits> {
        using BackendType = typename BackendTraits::CommandBufferType;
    };

    template<typename BackendTraits>
    struct ToBackendTraits<InputStateBase, BackendTraits> {
        using BackendType = typename BackendTraits::InputStateType;
    };

    template<typename BackendTraits>
    struct ToBackendTraits<PipelineBase, BackendTraits> {
        using BackendType = typename BackendTraits::PipelineType;
    };

    template<typename BackendTraits>
    struct ToBackendTraits<PipelineLayoutBase, BackendTraits> {
        using BackendType = typename BackendTraits::PipelineLayoutType;
    };

    template<typename BackendTraits>
    struct ToBackendTraits<QueueBase, BackendTraits> {
        using BackendType = typename BackendTraits::QueueType;
    };

    template<typename BackendTraits>
    struct ToBackendTraits<SamplerBase, BackendTraits> {
        using BackendType = typename BackendTraits::SamplerType;
    };

    template<typename BackendTraits>
    struct ToBackendTraits<ShaderModuleBase, BackendTraits> {
        using BackendType = typename BackendTraits::ShaderModuleType;
    };

    template<typename BackendTraits>
    struct ToBackendTraits<TextureBase, BackendTraits> {
        using BackendType = typename BackendTraits::TextureType;
    };

    template<typename BackendTraits>
    struct ToBackendTraits<TextureViewBase, BackendTraits> {
        using BackendType = typename BackendTraits::TextureViewType;
    };

    // ToBackendBase implements conversion to the given BackendTraits
    // To use it in a backend, use the following:
    //   template<typename T>
    //   auto ToBackend(T&& common) -> decltype(ToBackendBase<MyBackendTraits>(common)) {
    //       return ToBackendBase<MyBackendTraits>(common);
    //   }

    template<typename BackendTraits, typename T>
    Ref<typename ToBackendTraits<T, BackendTraits>::BackendType>& ToBackendBase(Ref<T>& common) {
        return reinterpret_cast<Ref<typename ToBackendTraits<T, BackendTraits>::BackendType>&>(common);
    }

    template<typename BackendTraits, typename T>
    const Ref<typename ToBackendTraits<T, BackendTraits>::BackendType>& ToBackendBase(const Ref<T>& common) {
        return reinterpret_cast<const Ref<typename ToBackendTraits<T, BackendTraits>::BackendType>&>(common);
    }

    template<typename BackendTraits, typename T>
    typename ToBackendTraits<T, BackendTraits>::BackendType* ToBackendBase(T* common) {
        return reinterpret_cast<typename ToBackendTraits<T, BackendTraits>::BackendType*>(common);
    }

    template<typename BackendTraits, typename T>
    const typename ToBackendTraits<T, BackendTraits>::BackendType* ToBackendBase(const T* common) {
        return reinterpret_cast<const typename ToBackendTraits<T, BackendTraits>::BackendType*>(common);
    }

}

#endif // BACKEND_COMMON_TOBACKEND_H_
