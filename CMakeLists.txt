# Copyright (c) 2017 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

cmake_minimum_required(VERSION 2.8)
project(nxt C CXX)

function(SetCXX14 Target)
    if(MSVC)
        set_property(TARGET ${Target} APPEND PROPERTY COMPILE_OPTIONS "/std:c++14")
    else()
        set_property(TARGET ${Target} APPEND PROPERTY COMPILE_OPTIONS "-std=c++14")
    endif()
endfunction()
function(SetPIC Target)
    if(MSVC)
    else()
        set_property(TARGET ${Target} APPEND PROPERTY COMPILE_OPTIONS "-fPIC")
    endif()
endfunction()

add_subdirectory(external)
add_subdirectory(generator)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src/include)

Generate(
    LIB_NAME nxt
    LIB_TYPE SHARED
    PRINT_NAME libNXT
    COMMAND_LINE_ARGS
        ${GENERATOR_COMMON_ARGS}
        -T nxt
)
target_include_directories(nxt PUBLIC ${GENERATED_DIR})

Generate(
    LIB_NAME nxtcpp
    LIB_TYPE SHARED
    PRINT_NAME libNXT++
    COMMAND_LINE_ARGS
        ${GENERATOR_COMMON_ARGS}
        -T nxtcpp
)
target_include_directories(nxtcpp PUBLIC ${GENERATED_DIR} PUBLIC ${INCLUDE_DIR})
target_link_libraries(nxtcpp nxt)
SetCXX14(nxtcpp)

Generate(
    LIB_NAME mock_nxt
    LIB_TYPE SHARED
    PRINT_NAME libMockNXT
    COMMAND_LINE_ARGS
        ${GENERATOR_COMMON_ARGS}
        -T mock_nxt
)
target_include_directories(mock_nxt PUBLIC ${GENERATED_DIR})
target_link_libraries(mock_nxt nxt gtest)
SetCXX14(mock_nxt)

add_subdirectory(src/backend)
add_subdirectory(src/wire)
add_subdirectory(src/tests)

add_subdirectory(examples)
