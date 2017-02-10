# Copyright (c) 2017 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

add_executable(glTFViewer glTFViewer.cpp)
target_link_libraries(glTFViewer utils)
target_include_directories(glTFViewer PUBLIC ../ ${GLM_INCLUDE_DIR})
SetCXX14(glTFViewer)
