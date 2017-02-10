// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WIRE_WIRE_H_
#define WIRE_WIRE_H_

#include <cstdint>

#include "nxt/nxt.h"

namespace client {
    class CommandSerializer {
        public:
            virtual ~CommandSerializer() = default;
            virtual void* GetCmdSpace(size_t size) = 0;
            virtual void Flush() = 0;
    };

    void NewClientDevice(nxtProcTable* procs, nxtDevice* device, CommandSerializer* serializer);
}

namespace server {
    class CommandHandler {
        public:
            virtual ~CommandHandler() = default;
            virtual const uint8_t* HandleCommands(const uint8_t* commands, size_t size) = 0;

            virtual void OnSynchronousError() = 0;
    };

    CommandHandler* CreateCommandHandler(nxtDevice device, const nxtProcTable& procs);
}

#endif // WIRE_WIRE_H_
