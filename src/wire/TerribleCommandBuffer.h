// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WIRE_TERRIBLE_COMMAND_BUFFER_H_
#define WIRE_TERRIBLE_COMMAND_BUFFER_H_

#include <vector>

#include "Wire.h"

class TerribleCommandBuffer : public client::CommandSerializer {
    public:
        TerribleCommandBuffer(server::CommandHandler* handler) : handler(handler) {
        }
        void* GetCmdSpace(size_t size) {
            if (size > sizeof(buffer)) {
                return nullptr;
            }

            uint8_t* result = &buffer[offset];
            offset += size;

            if (offset > sizeof(buffer)) {
                Flush();
                return GetCmdSpace(size);
            }

            return result;
        }
        void Flush() {
            handler->HandleCommands(buffer, offset);
            offset = 0;
        }

    private:
        server::CommandHandler* handler = nullptr;
        size_t offset;
        uint8_t buffer[10000000];
};

#endif // WIRE_TERRIBLE_COMMAND_BUFFER_H_
