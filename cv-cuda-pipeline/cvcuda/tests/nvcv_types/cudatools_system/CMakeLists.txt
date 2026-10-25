# SPDX-FileCopyrightText: Copyright (c) 2022-2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

project(nvcv_test_cudatools_system)
set(CMAKE_FOLDER tests)

# Base source files that work with C++17
set(CUDATOOLS_TEST_SOURCES
    TestAtomics.cpp
    DeviceAtomics.cu
    TestBorderVarShapeWrap.cpp
    DeviceBorderVarShapeWrap.cu
    TestImageBatchVarShapeWrap.cpp
    DeviceImageBatchVarShapeWrap.cu
    TestBorderWrap.cpp
    DeviceBorderWrap.cu
    TestTypeTraits.cpp
    TestMetaprogramming.cpp
    TestTensorBatchWrap.cpp
    DeviceTensorBatchWrap.cu
)

# Test files that require C++20 (use complex types as template non-type parameters)
# These are incompatible with GCC-10 which doesn't fully support structural types for NTTPs
set(CUDATOOLS_TEST_SOURCES_CPP20
    TestInterpolationVarShapeWrap.cpp
    DeviceInterpolationVarShapeWrap.cu
    TestInterpolationWrap.cpp
    DeviceInterpolationWrap.cu
    TestFullTensorWrap.cpp
    DeviceFullTensorWrap.cu
    TestLinAlg.cpp
    TestTensorWrap.cpp
    DeviceTensorWrap.cu
    TestMathOps.cpp
    TestArrayWrap.cpp
    TestDropCast.cpp
    TestMathWrappers.cpp
    DeviceMathWrappers.cu
    TestRangeCast.cpp
    TestSaturateCast.cpp
    DeviceSaturateCast.cu
    TestStaticCast.cpp
)

# Include C++20-requiring tests only if compiler supports them properly
if(NOT (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 11.0))
    list(APPEND CUDATOOLS_TEST_SOURCES ${CUDATOOLS_TEST_SOURCES_CPP20})
else()
    message(WARNING "GCC-${CMAKE_CXX_COMPILER_VERSION}: Excluding cudatools tests requiring C++20 NTTP features. Build with GCC-11+ for full test coverage.")
endif()

add_executable(nvcv_test_cudatools_system ${CUDATOOLS_TEST_SOURCES})

target_link_libraries(nvcv_test_cudatools_system
    PUBLIC
        nvcv_types
        nvcv_util
        nvcv_test_common_system
)

target_include_directories(nvcv_test_cudatools_system
    PRIVATE
        ${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}
)

nvcv_add_test(nvcv_test_cudatools_system nvcv cpp)
