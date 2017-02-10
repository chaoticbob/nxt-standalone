// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <gtest/gtest.h>

#include "common/PerStage.h"

using namespace backend;

// Tests for StageBit
TEST(PerStage, StageBit) {
    ASSERT_EQ(StageBit(nxt::ShaderStage::Vertex), nxt::ShaderStageBit::Vertex);
    ASSERT_EQ(StageBit(nxt::ShaderStage::Fragment), nxt::ShaderStageBit::Fragment);
    ASSERT_EQ(StageBit(nxt::ShaderStage::Compute), nxt::ShaderStageBit::Compute);
}

// Basic test for the PerStage container
TEST(PerStage, PerStage) {
    PerStage<int> data;

    // Store data using nxt::ShaderStage
    data[nxt::ShaderStage::Vertex] = 42;
    data[nxt::ShaderStage::Fragment] = 3;
    data[nxt::ShaderStage::Compute] = -1;

    // Load it using nxt::ShaderStageBit
    ASSERT_EQ(data[nxt::ShaderStageBit::Vertex], 42);
    ASSERT_EQ(data[nxt::ShaderStageBit::Fragment], 3);
    ASSERT_EQ(data[nxt::ShaderStageBit::Compute], -1);
}

// Test IterateStages with kAllStages
TEST(PerStage, IterateAllStages) {
    PerStage<int> counts;
    counts[nxt::ShaderStage::Vertex] = 0;
    counts[nxt::ShaderStage::Fragment] = 0;
    counts[nxt::ShaderStage::Compute] = 0;

    for (auto stage : IterateStages(kAllStages)) {
        counts[stage] ++;
    }

    ASSERT_EQ(counts[nxt::ShaderStageBit::Vertex], 1);
    ASSERT_EQ(counts[nxt::ShaderStageBit::Fragment], 1);
    ASSERT_EQ(counts[nxt::ShaderStageBit::Compute], 1);
}

// Test IterateStages with one stage
TEST(PerStage, IterateOneStage) {
    PerStage<int> counts;
    counts[nxt::ShaderStage::Vertex] = 0;
    counts[nxt::ShaderStage::Fragment] = 0;
    counts[nxt::ShaderStage::Compute] = 0;

    for (auto stage : IterateStages(nxt::ShaderStageBit::Fragment)) {
        counts[stage] ++;
    }

    ASSERT_EQ(counts[nxt::ShaderStageBit::Vertex], 0);
    ASSERT_EQ(counts[nxt::ShaderStageBit::Fragment], 1);
    ASSERT_EQ(counts[nxt::ShaderStageBit::Compute], 0);
}

// Test IterateStages with no stage
TEST(PerStage, IterateNoStages) {
    PerStage<int> counts;
    counts[nxt::ShaderStage::Vertex] = 0;
    counts[nxt::ShaderStage::Fragment] = 0;
    counts[nxt::ShaderStage::Compute] = 0;

    for (auto stage : IterateStages(nxt::ShaderStageBit::Fragment & nxt::ShaderStageBit::Vertex)) {
        counts[stage] ++;
    }

    ASSERT_EQ(counts[nxt::ShaderStageBit::Vertex], 0);
    ASSERT_EQ(counts[nxt::ShaderStageBit::Fragment], 0);
    ASSERT_EQ(counts[nxt::ShaderStageBit::Compute], 0);
}
