// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_COMMON_COMMANDS_H_
#define BACKEND_COMMON_COMMANDS_H_

#include "Texture.h"

#include "nxt/nxtcpp.h"

namespace backend {

    // Definition of the commands that are present in the CommandIterator given by the
    // CommandBufferBuilder. There are not defined in CommandBuffer.h to break some header
    // dependencies: Ref<Object> needs Object to be defined.

    enum class Command {
        CopyBufferToTexture,
        Dispatch,
        DrawArrays,
        DrawElements,
        SetPipeline,
        SetPushConstants,
        SetBindGroup,
        SetIndexBuffer,
        SetVertexBuffers,
    };

    struct CopyBufferToTextureCmd {
        Ref<BufferBase> buffer;
        Ref<TextureBase> texture;
        uint32_t x, y, z;
        uint32_t width, height, depth;
        uint32_t level;
    };

    struct DispatchCmd {
        uint32_t x;
        uint32_t y;
        uint32_t z;
    };

    struct DrawArraysCmd {
        uint32_t vertexCount;
        uint32_t instanceCount;
        uint32_t firstVertex;
        uint32_t firstInstance;
    };

    struct DrawElementsCmd {
        uint32_t indexCount;
        uint32_t instanceCount;
        uint32_t firstIndex;
        uint32_t firstInstance;
    };

    struct SetPipelineCmd {
        Ref<PipelineBase> pipeline;
    };

    struct SetPushConstantsCmd {
        nxt::ShaderStageBit stage;
        uint32_t offset;
        uint32_t count;
    };

    struct SetBindGroupCmd {
        uint32_t index;
        Ref<BindGroupBase> group;
    };

    struct SetIndexBufferCmd {
        Ref<BufferBase> buffer;
        uint32_t offset;
        nxt::IndexFormat format;
    };

    struct SetVertexBuffersCmd {
        uint32_t startSlot;
        uint32_t count;
    };

    // This needs to be called before the CommandIterator is freed so that the Ref<> present in
    // the commands have a chance to run their destructor and remove internal references.
    void FreeCommands(CommandIterator* commands);

}

#endif // BACKEND_COMMON_COMMANDS_H_
