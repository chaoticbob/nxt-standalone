// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "BackendBinding.h"

#define GLFW_EXPOSE_NATIVE_COCOA
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

#import <QuartzCore/CAMetalLayer.h>
#import <Metal/Metal.h>

namespace backend {
namespace metal {
    void Init(id<MTLDevice> metalDevice, nxtProcTable* procs, nxtDevice* device);
    void SetNextDrawable(nxtDevice device, id<CAMetalDrawable> drawable);
    void Present(nxtDevice device);
}
}

class MetalBinding : public BackendBinding {
    public:
        void SetupGLFWWindowHints() override {
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        }
        void GetProcAndDevice(nxtProcTable* procs, nxtDevice* device) override {
            metalDevice = MTLCreateSystemDefaultDevice();

            id nsWindow = glfwGetCocoaWindow(window);
            NSView* contentView = [nsWindow contentView];
            [contentView setWantsLayer: YES];

            layer = [CAMetalLayer layer];
            [layer setDevice: metalDevice];
            [layer setPixelFormat: MTLPixelFormatBGRA8Unorm];
            [layer setFramebufferOnly: YES];
            [layer setDrawableSize: [contentView bounds].size];

            [contentView setLayer: layer];

            backend::metal::Init(metalDevice, procs, device);
            backendDevice = *device;

            backend::metal::SetNextDrawable(backendDevice, GetNextDrawable());
        }
        void SwapBuffers() override {
            backend::metal::Present(backendDevice);
            backend::metal::SetNextDrawable(backendDevice, GetNextDrawable());
        }

    private:
        id<CAMetalDrawable> GetNextDrawable() {
            lastDrawable = [layer nextDrawable];
            return lastDrawable;
        }

        id<MTLDevice> metalDevice = nil;
        CAMetalLayer* layer = nullptr;

        id<CAMetalDrawable> lastDrawable = nil;

        nxtDevice backendDevice = nullptr;
};

BackendBinding* CreateMetalBinding() {
    return new MetalBinding;
}
