# MIT License
# 
# Copyright (c) 2025 RealTimeChris (Chris M)
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "FT-TL Library"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# This file was independently created by RealTimeChris (Chris M), without reuse
# or derivation from any codebase owned by other entities, including any contract work.
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
# INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
# AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
# FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
# OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
# OR OTHER DEALINGS IN THE SOFTWARE.
# https://github.com/RealTimeChris/rt_tm

message(STATUS "FT-TL: Configuring AVX2 optimizations")

file(GLOB_RECURSE RT_TM_AVX2_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(rt_tm_avx2 STATIC ${RT_TM_AVX2_SOURCES})
add_library(rt_tm::avx2 ALIAS rt_tm_avx2)

rt_tm_configure_x64_variant(rt_tm_avx2)

target_compile_options(rt_tm_avx2 PRIVATE
    "$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-mavx2>"
    "$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-mfma>"
    "$<$<CXX_COMPILER_ID:MSVC>:/arch:AVX2>"
)
