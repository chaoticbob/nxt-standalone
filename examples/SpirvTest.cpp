// Copyright (c) 2017 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <iostream>

#include <nxt/nxtcpp.h>
#include <shaderc/shaderc.hpp>
#include <spirv-cross/spirv_glsl.hpp>

bool CompileToSpv(std::string source, nxt::ShaderStage stage, std::vector<uint32_t>* spv) {
    shaderc::Compiler compiler;
    shaderc::CompileOptions options;

    shaderc_shader_kind kind;
    switch (stage) {
        case nxt::ShaderStage::Vertex:
            kind = shaderc_glsl_vertex_shader;
            break;
        case nxt::ShaderStage::Fragment:
            kind = shaderc_glsl_fragment_shader;
            break;
        default:
            return false;
    }

    {
        auto result = compiler.CompileGlslToSpvAssembly(source.c_str(), source.size(), kind, "toto", options);
        if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
            std::cerr << result.GetErrorMessage();
            return false;
        }

        std::cout << "*** Begin spirv assembly" << std::endl;
        std::cout << std::string(result.cbegin(), result.cend()) << std::endl;
        std::cout << "*** End spirv assembly" << std::endl;
    }

    auto result = compiler.CompileGlslToSpv(source.c_str(), source.size(), kind, "toto", options);
    if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
        std::cerr << result.GetErrorMessage();
        return false;
    }

    spv->assign(result.cbegin(), result.cend());

    return true;
}

void TestSpv(std::string source, nxt::ShaderStage stage) {
    std::vector<uint32_t> spv;
    if (!CompileToSpv(source, stage, &spv)) {
        return;
    }

    spirv_cross::CompilerGLSL glsl(std::move(spv));

    spirv_cross::CompilerGLSL::Options options;
    options.version = 450;
    options.es = false;
    glsl.set_options(options);

    std::cout << "*** Begin glsl cross" << std::endl;
    std::cout << glsl.compile() << std::endl;
    std::cout << "*** End glsl cross" << std::endl;
}

int main(int argc, char const* const* argv) {
    const char* vs =
        "#version 450\n"
        "layout(push_constant) uniform toto {int foo;} tata;\n"
        "const vec2 pos[3] = vec2[3](vec2(0.0f, 0.5f), vec2(-0.5f, -0.5f), vec2(0.5f, -0.5f));\n"
        "void main() {\n"
        "   gl_Position = vec4(pos[gl_VertexIndex], 0.0, 1.0);\n"
        "}\n";
    TestSpv(vs, nxt::ShaderStage::Vertex);

    return 0;
}
