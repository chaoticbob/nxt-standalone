# Copyright (c) 2017 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(nxt_unittests
    ${TESTS_DIR}/EnumClassBitmasksTests.cpp
    ${TESTS_DIR}/UnittestsMain.cpp
)
target_link_libraries(nxt_unittests gtest nxtcpp)
SetCXX14(nxt_unittests)
