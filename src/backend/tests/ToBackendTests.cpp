// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <gtest/gtest.h>

#include "common/RefCounted.h"
#include "common/ToBackend.h"

#include <type_traits>

// Make our own Base - Backend object pair, reusing the CommandBuffer name
namespace backend {
    class CommandBufferBase : public RefCounted {
    };
}

using namespace backend;

class MyCommandBuffer : public CommandBufferBase {
};

struct MyBackendTraits {
    using CommandBufferType = MyCommandBuffer;
};

// Instanciate ToBackend for our "backend"
template<typename T>
auto ToBackend(T&& common) -> decltype(ToBackendBase<MyBackendTraits>(common)) {
    return ToBackendBase<MyBackendTraits>(common);
}

// Test that ToBackend correctly converts pointers to base classes.
TEST(ToBackend, Pointers) {
    {
        MyCommandBuffer* cmdBuf = new MyCommandBuffer;
        const CommandBufferBase* base = cmdBuf;

        auto backendCmdBuf = ToBackend(base);
        static_assert(std::is_same<decltype(backendCmdBuf), const MyCommandBuffer*>::value, "");
        ASSERT_EQ(cmdBuf, backendCmdBuf);

        cmdBuf->Release();
    }
    {
        MyCommandBuffer* cmdBuf = new MyCommandBuffer;
        CommandBufferBase* base = cmdBuf;

        auto backendCmdBuf = ToBackend(base);
        static_assert(std::is_same<decltype(backendCmdBuf), MyCommandBuffer*>::value, "");
        ASSERT_EQ(cmdBuf, backendCmdBuf);

        cmdBuf->Release();
    }
}

// Test that ToBackend correctly converts Refs to base classes.
TEST(ToBackend, Ref) {
    {
        MyCommandBuffer* cmdBuf = new MyCommandBuffer;
        const Ref<CommandBufferBase> base(cmdBuf);

        const auto& backendCmdBuf = ToBackend(base);
        static_assert(std::is_same<decltype(ToBackend(base)), const Ref<MyCommandBuffer>&>::value, "");
        ASSERT_EQ(cmdBuf, backendCmdBuf.Get());

        cmdBuf->Release();
    }
    {
        MyCommandBuffer* cmdBuf = new MyCommandBuffer;
        Ref<CommandBufferBase> base(cmdBuf);

        auto backendCmdBuf = ToBackend(base);
        static_assert(std::is_same<decltype(ToBackend(base)), Ref<MyCommandBuffer>&>::value, "");
        ASSERT_EQ(cmdBuf, backendCmdBuf.Get());

        cmdBuf->Release();
    }
}
