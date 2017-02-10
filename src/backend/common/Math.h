// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_COMMON_MATH_H_
#define BACKEND_COMMON_MATH_H_

#include <cstddef>
#include "cstdint"

namespace backend {

    // The following are not valid for 0
    unsigned long ScanForward(unsigned long bits);
    uint32_t Log2(uint32_t value);
    bool IsPowerOfTwo(size_t n);

    bool IsAligned(const void* ptr, size_t alignment);
    void* AlignVoidPtr(void* ptr, size_t alignment);

    template<typename T>
    T* Align(T* ptr, size_t alignment) {
        return reinterpret_cast<T*>(AlignVoidPtr(ptr, alignment));
    }

    template<typename T>
    const T* Align(const T* ptr, size_t alignment) {
        return reinterpret_cast<const T*>(AlignVoidPtr(const_cast<T*>(ptr), alignment));
    }

}

#endif // BACKEND_COMMON_MATH_H_
