# Copyright (c) 2017 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

set(WIRE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

Generate(
    LIB_NAME wire_autogen
    LIB_TYPE STATIC
    PRINT_NAME "Wire serialization/deserialization autogenerated files"
    COMMAND_LINE_ARGS
        ${GENERATOR_COMMON_ARGS}
        -T wire
)
target_include_directories(wire_autogen PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(wire_autogen PUBLIC ${GENERATED_DIR})
SetCXX14(wire_autogen)
SetPic(wire_autogen)

add_library(nxt_wire SHARED
    ${WIRE_DIR}/TerribleCommandBuffer.h
)
target_link_libraries(nxt_wire wire_autogen)
SetCXX14(nxt_wire)

add_executable(wire_unittests
    ${TESTS_DIR}/UnittestsMain.cpp
    ${TESTS_DIR}/WireTests.cpp
)
target_link_libraries(wire_unittests mock_nxt nxt_wire)
target_include_directories(wire_unittests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
SetCXX14(wire_unittests)
