// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_COMMON_PERSTAGE_H_
#define BACKEND_COMMON_PERSTAGE_H_

#include "BitSetIterator.h"

#include "nxt/nxtcpp.h"

#include <array>

namespace backend {

    static_assert(static_cast<uint32_t>(nxt::ShaderStage::Vertex) < kNumStages, "");
    static_assert(static_cast<uint32_t>(nxt::ShaderStage::Fragment) < kNumStages, "");
    static_assert(static_cast<uint32_t>(nxt::ShaderStage::Compute) < kNumStages, "");

    static_assert(static_cast<uint32_t>(nxt::ShaderStageBit::Vertex) == (1 << static_cast<uint32_t>(nxt::ShaderStage::Vertex)), "");
    static_assert(static_cast<uint32_t>(nxt::ShaderStageBit::Fragment) == (1 << static_cast<uint32_t>(nxt::ShaderStage::Fragment)), "");
    static_assert(static_cast<uint32_t>(nxt::ShaderStageBit::Compute) == (1 << static_cast<uint32_t>(nxt::ShaderStage::Compute)), "");

    BitSetIterator<kNumStages, nxt::ShaderStage> IterateStages(nxt::ShaderStageBit stages);
    nxt::ShaderStageBit StageBit(nxt::ShaderStage stage);

    static constexpr nxt::ShaderStageBit kAllStages = static_cast<nxt::ShaderStageBit>((1 << kNumStages) - 1);

    template<typename T>
    class PerStage {
        public:
            T& operator[](nxt::ShaderStage stage) {
                ASSERT(static_cast<uint32_t>(stage) < kNumStages);
                return data[static_cast<uint32_t>(stage)];
            }
            const T& operator[](nxt::ShaderStage stage) const {
                ASSERT(static_cast<uint32_t>(stage) < kNumStages);
                return data[static_cast<uint32_t>(stage)];
            }

            T& operator[](nxt::ShaderStageBit stageBit) {
                uint32_t bit = static_cast<uint32_t>(stageBit);
                ASSERT(bit != 0 && IsPowerOfTwo(bit) && bit <= (1 << kNumStages));
                return data[Log2(bit)];
            }
            const T& operator[](nxt::ShaderStageBit stageBit) const {
                uint32_t bit = static_cast<uint32_t>(stageBit);
                ASSERT(bit != 0 && IsPowerOfTwo(bit) && bit <= (1 << kNumStages));
                return data[Log2(bit)];
            }

        private:
            std::array<T, kNumStages> data;
    };

}

#endif // BACKEND_COMMON_PERSTAGE_H_
