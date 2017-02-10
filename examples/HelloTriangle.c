// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "Utils.h"

#include <unistd.h>

nxtDevice device;
nxtQueue queue;
nxtPipeline pipeline;

void init() {
    nxtProcTable procs;
    GetProcTableAndDevice(&procs, &device);
    nxtSetProcs(&procs);

    {
        nxtQueueBuilder builder = nxtDeviceCreateQueueBuilder(device);
        queue = nxtQueueBuilderGetResult(builder);
        nxtQueueBuilderRelease(builder);
    }

    const char* vs =
        "#version 450\n"
        "const vec2 pos[3] = vec2[3](vec2(0.0f, 0.5f), vec2(-0.5f, -0.5f), vec2(0.5f, -0.5f));\n"
        "void main() {\n"
        "   gl_Position = vec4(pos[gl_VertexIndex], 0.0, 1.0);\n"
        "}\n";
    nxtShaderModule vsModule = CreateShaderModule(device, NXT_SHADER_STAGE_VERTEX, vs);

    const char* fs =
        "#version 450\n"
        "out vec4 fragColor;"
        "void main() {\n"
        "   fragColor = vec4(1.0, 0.0, 0.0, 1.0);\n"
        "}\n";
    nxtShaderModule fsModule = CreateShaderModule(device, NXT_SHADER_STAGE_FRAGMENT, fs);

    {
        nxtPipelineBuilder builder = nxtDeviceCreatePipelineBuilder(device);
        nxtPipelineBuilderSetStage(builder, NXT_SHADER_STAGE_VERTEX, vsModule, "main");
        nxtPipelineBuilderSetStage(builder, NXT_SHADER_STAGE_FRAGMENT, fsModule, "main");
        pipeline = nxtPipelineBuilderGetResult(builder);
        nxtPipelineBuilderRelease(builder);
    }

    nxtShaderModuleRelease(vsModule);
    nxtShaderModuleRelease(fsModule);
}

void frame() {
    nxtCommandBuffer commands;
    {
        nxtCommandBufferBuilder builder = nxtDeviceCreateCommandBufferBuilder(device);
        nxtCommandBufferBuilderSetPipeline(builder, pipeline);
        nxtCommandBufferBuilderDrawArrays(builder, 3, 1, 0, 0);
        commands = nxtCommandBufferBuilderGetResult(builder);
        nxtCommandBufferBuilderRelease(builder);
    }

    nxtQueueSubmit(queue, 1, &commands);
    nxtCommandBufferRelease(commands);

    SwapBuffers();
}

int main(int argc, const char* argv[]) {
    if (!InitUtils(argc, argv)) {
        return 1;
    }
    init();

    while (!ShouldQuit()) {
        frame();
        usleep(16000);
    }

    // TODO release stuff
}
