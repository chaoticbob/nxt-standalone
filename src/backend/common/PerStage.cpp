// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "PerStage.h"

namespace backend {

    BitSetIterator<kNumStages, nxt::ShaderStage> IterateStages(nxt::ShaderStageBit stages) {
        std::bitset<kNumStages> bits(static_cast<uint32_t>(stages));
        return BitSetIterator<kNumStages, nxt::ShaderStage>(bits);
    }

    nxt::ShaderStageBit StageBit(nxt::ShaderStage stage) {
        ASSERT(static_cast<uint32_t>(stage) < kNumStages);
        return static_cast<nxt::ShaderStageBit>(1 << static_cast<uint32_t>(stage));
    }

}
