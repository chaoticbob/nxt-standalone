// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "Math.h"

#include "Forward.h"

namespace backend {

    unsigned long ScanForward(unsigned long bits) {
        ASSERT(bits != 0);
        // TODO(cwallez@chromium.org): handle non-posix platforms
        // unsigned long firstBitIndex = 0ul;
        // unsigned char ret = _BitScanForward(&firstBitIndex, bits);
        // ASSERT(ret != 0);
        // return firstBitIndex;
        return static_cast<unsigned long>(__builtin_ctzl(bits));
    }

    uint32_t Log2(uint32_t value) {
        ASSERT(value != 0);
        return 31 - __builtin_clz(value);
    }

    bool IsPowerOfTwo(size_t n) {
        ASSERT(n != 0);
        return (n & (n - 1)) == 0;
    }

    bool IsAligned(const void* ptr, size_t alignment) {
        ASSERT(IsPowerOfTwo(alignment));
        ASSERT(alignment != 0);
        return (reinterpret_cast<intptr_t>(ptr) & (alignment - 1)) == 0;
    }

    void* AlignVoidPtr(void* ptr, size_t alignment) {
        ASSERT(alignment != 0);
        return reinterpret_cast<void*>((reinterpret_cast<intptr_t>(ptr) + (alignment - 1)) & ~(alignment - 1));
    }

}
