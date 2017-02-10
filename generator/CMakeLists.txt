# Copyright (c) 2017 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

find_package(PythonInterp REQUIRED)
include(CMakeParseArguments)

# Check for Jinja2
message(STATUS "${PYTHON_EXECUTABLE}")
execute_process(
    COMMAND ${PYTHON_EXECUTABLE} -c "import jinja2"
    RESULT_VARIABLE RET
)
if (NOT RET EQUAL 0)
    message(FATAL_ERROR "Missing dependencies for VkCPP generation, please ensure you have python-jinja2 installed.")
endif()

function(Generate)
    set(oneValueArgs LIB_NAME LIB_TYPE PRINT_NAME EXECUTABLE)
    set(multiValueArgs COMMAND_LINE_ARGS EXTRA_DEPS SOURCE)
    cmake_parse_arguments(G "" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    execute_process(
        COMMAND ${G_COMMAND_LINE_ARGS} --print-dependencies
        OUTPUT_VARIABLE DEPENDENCIES
        RESULT_VARIABLE RET
    )
    if (NOT RET EQUAL 0)
        message(STATUS ${RET})
        message(FATAL_ERROR "Failed to get the dependencies for ${G_PRINT_NAME}.")
    endif()

    execute_process(
        COMMAND ${G_COMMAND_LINE_ARGS} --print-outputs
        OUTPUT_VARIABLE OUTPUTS
        RESULT_VARIABLE RET
    )
    if (NOT RET EQUAL 0)
        message(FATAL_ERROR "Failed to get the outputs for ${G_PRINT_NAME}.")
    endif()

    add_custom_command(
        COMMAND ${G_COMMAND_LINE_ARGS}
        DEPENDS ${DEPENDENCIES} ${G_EXTRA_DEPS}
        OUTPUT ${OUTPUTS}
        COMMENT "Generating files for ${G_PRINT_NAME}."
    )

    add_library(${G_LIB_NAME} ${G_LIB_TYPE}
        ${G_SOURCE}
        ${OUTPUTS}
    )
endfunction()

set(GENERATED_DIR ${CMAKE_CURRENT_BINARY_DIR} PARENT_SCOPE)

set(GENERATOR_COMMON_ARGS
    ${PYTHON_EXECUTABLE}
    ${CMAKE_CURRENT_SOURCE_DIR}/main.py
    ${CMAKE_SOURCE_DIR}/next.json
    -t ${CMAKE_CURRENT_SOURCE_DIR}/templates
    -o ${CMAKE_CURRENT_BINARY_DIR}
    PARENT_SCOPE
)
