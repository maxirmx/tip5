# Copyright (c) 2025 Maxim [maxirmx] Samsonov
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
#
# This file is a part of tip5xx library
#

cmake_minimum_required(VERSION 3.15)
project(tip5xx VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Options
option(BUILD_TESTING "Build tests" ON)
option(BUILD_SAMPLES "Build samples" ON)
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)
option(ENABLE_SANITIZER "Enable Address Sanitizer" OFF)

# Sanitizer
if(ENABLE_SANITIZER)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address)
    else()
        message(FATAL_ERROR "Address Sanitizer requires GCC or Clang")
    endif()
endif()

# Coverage
if(ENABLE_COVERAGE)
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(WARNING "Code coverage results with an optimized (non-Debug) build may be misleading")
    endif()

    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(-O0 -g --coverage)
        add_link_options(--coverage)
    else()
        message(FATAL_ERROR "Code coverage requires GCC or Clang")
    endif()
endif()

if(MSVC AND CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    # Find clang installation directory
    execute_process(
        COMMAND ${CMAKE_CXX_COMPILER} --print-resource-dir
        OUTPUT_VARIABLE CLANG_RESOURCE_DIR
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )

    # Construct the path to the builtins library for x86_64
    if(CLANG_RESOURCE_DIR)
        set(BUILTINS_LIB "${CLANG_RESOURCE_DIR}/lib/windows/clang_rt.builtins-x86_64.lib")
        message(STATUS "Using compiler-rt builtins library: ${BUILTINS_LIB}")

        # Check if the file exists
        if(EXISTS "${BUILTINS_LIB}")
            link_libraries("${BUILTINS_LIB}")
        else()
            message(WARNING "Could not find compiler-rt builtins library at ${BUILTINS_LIB}")

            # Fallback to looking in the lib/clang directory
            file(GLOB CLANG_VERSION_DIRS "${CLANG_RESOURCE_DIR}/lib/clang/*")
            if(CLANG_VERSION_DIRS)
                list(GET CLANG_VERSION_DIRS 0 CLANG_VERSION_DIR)
                set(FALLBACK_LIB "${CLANG_VERSION_DIR}/lib/windows/clang_rt.builtins-x86_64.lib")

                if(EXISTS "${FALLBACK_LIB}")
                    message(STATUS "Found fallback builtins library at ${FALLBACK_LIB}")
                    link_libraries("${FALLBACK_LIB}")
                else()
                    message(FATAL_ERROR "Could not find compiler-rt builtins library")
                endif()
            endif()
        endif()
    else()
        message(FATAL_ERROR "Could not determine Clang resource directory")
    endif()
endif()

# Library
add_subdirectory(lib)

# Tests
if(BUILD_TESTING)
    enable_testing()
    include(FetchContent)
    FetchContent_Declare(
        googletest
        GIT_REPOSITORY https://github.com/google/googletest.git
        GIT_TAG v1.14.0
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    # Disable GMock as we don't use it
    set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    add_subdirectory(tests)
endif()

# Samples
if(BUILD_SAMPLES)
    add_subdirectory(samples)
endif()
