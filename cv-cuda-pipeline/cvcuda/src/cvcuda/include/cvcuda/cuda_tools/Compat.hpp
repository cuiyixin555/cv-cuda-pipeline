/*
 * SPDX-FileCopyrightText: Copyright (c) 2025 NVIDIA CORPORATION & AFFILIATES. All rights reserved.
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file Compat.hpp
 *
 * @brief Defines types for forward compatibility with CUDA 13.0.
 */

#ifndef NVCV_CUDA_COMPAT_HPP
#define NVCV_CUDA_COMPAT_HPP

#include <cuda.h>
#include <vector_types.h>

// define 16a and 32a aliases for CUDA compound types prior to CUDA 13.0
#if CUDA_VERSION < 13000

using double4_16a    = double4;
using long4_16a      = long4;
using ulong4_16a     = ulong4;
using longlong4_16a  = longlong4;
using ulonglong4_16a = ulonglong4;

struct alignas(32) double4_32a
{
    double x, y, z, w;
};

struct alignas(32) long4_32a
{
    long x, y, z, w;
};

struct alignas(32) ulong4_32a
{
    unsigned long x, y, z, w;
};

struct alignas(32) longlong4_32a
{
    long long x, y, z, w;
};

struct alignas(32) ulonglong4_32a
{
    unsigned long long x, y, z, w;
};

#endif

#endif //NVCV_CUDA_COMPAT_HPP
