// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <nxt/nxt.h>

#if defined(__cplusplus)
extern "C" {
#endif
    bool InitUtils(int argc, const char** argv);
    void SwapBuffers();
    bool ShouldQuit();

    struct GLFWwindow;
    struct GLFWwindow* GetWindow();
#if defined(__cplusplus)
}
#endif

// Yuck
#if defined(__cplusplus)
    #include <nxt/nxtcpp.h>
    void GetProcTableAndDevice(nxtProcTable* procs, nxt::Device* device);
    nxt::ShaderModule CreateShaderModule(const nxt::Device& device, nxt::ShaderStage stage, const char* source);
#else
    void GetProcTableAndDevice(nxtProcTable* procs, nxtDevice* device);
    nxtShaderModule CreateShaderModule(nxtDevice device, nxtShaderStage stage, const char* source);
#endif
