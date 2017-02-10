// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef BACKEND_OPENGL_COMMANDBUFFER_H_
#define BACKEND_OPENGL_COMMANDBUFFER_H_

#include "common/CommandAllocator.h"
#include "common/CommandBuffer.h"

namespace backend {
    class CommandBufferBuilder;
}

namespace backend {
namespace opengl {

    class  Device;

    class CommandBuffer : public CommandBufferBase {
        public:
            CommandBuffer(Device* device, CommandBufferBuilder* builder);
            ~CommandBuffer();

            void Execute();

        private:
            Device* device;
            CommandIterator commands;
    };

}
}

#endif // BACKEND_OPENGL_COMMANDBUFFER_H_
