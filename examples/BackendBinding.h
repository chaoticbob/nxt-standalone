// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UTILS_BACKENDBINDING_H_
#define UTILS_BACKENDBINDING_H_

#include <nxt/nxt.h>

struct GLFWwindow;

class BackendBinding {
    public:
        virtual void SetupGLFWWindowHints() = 0;
        virtual void GetProcAndDevice(nxtProcTable* procs, nxtDevice* device) = 0;
        virtual void SwapBuffers() = 0;

        void SetWindow(GLFWwindow* window) {this->window = window;}

    protected:
        GLFWwindow* window = nullptr;
};

#endif // UTILS_BACKENDBINDING_H_
