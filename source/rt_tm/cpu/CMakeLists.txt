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

message(STATUS "FT-TL: Configuring CPU variant subdirectories with centralized flags")

set(RT_TM_CPU_COMMON_COMPILE_FLAGS
    "$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-O3>"
    "$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-ffast-math>"
    "$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-funroll-loops>"
    "$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-fprefetch-loop-arrays>"

    "$<$<CONFIG:RELEASE>:$<$<CXX_COMPILER_ID:MSVC>:/GL>>"
    "$<$<CONFIG:RELEASE>:$<$<CXX_COMPILER_ID:MSVC>:/Gy>>"
    "$<$<CONFIG:RELEASE>:$<$<CXX_COMPILER_ID:MSVC>:/O2>>"
    "$<$<CONFIG:RELEASE>:$<$<CXX_COMPILER_ID:MSVC>:/Ob>>"
    "$<$<CONFIG:RELEASE>:$<$<CXX_COMPILER_ID:MSVC>:/fp:fast>>"
)

set(RT_TM_X64_BIT_MANIPULATION_FLAGS
    "$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-mbmi>"
    "$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-mbmi2>"
    "$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-mlzcnt>"
)

set(RT_TM_ARM_COMMON_FLAGS
    "$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},arm>>:-mfloat-abi=hard>"
    "$<$<AND:$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>,$<STREQUAL:${CMAKE_SYSTEM_PROCESSOR},aarch64>>:-march=armv8-a>"
)

function(rt_tm_configure_cpu_variant target_name)
    target_compile_options(${target_name} PRIVATE
        ${RT_TM_CPU_COMMON_COMPILE_FLAGS}
    )
    
    target_include_directories(${target_name} PRIVATE
        "${CMAKE_SOURCE_DIR}/include"
    )
    
    set_target_properties(${target_name} PROPERTIES
        CXX_STANDARD 23
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF
        POSITION_INDEPENDENT_CODE ON
    )
endfunction()

function(rt_tm_configure_x64_variant target_name)
    rt_tm_configure_cpu_variant(${target_name})
    
    target_compile_options(${target_name} PRIVATE
        ${RT_TM_X64_BIT_MANIPULATION_FLAGS}
    )
endfunction()

function(rt_tm_configure_arm_variant target_name)
    rt_tm_configure_cpu_variant(${target_name})
    
    target_compile_options(${target_name} PRIVATE
        ${RT_TM_ARM_COMMON_FLAGS}
    )
endfunction()

if(RT_TM_ARCH_X64)
    message(STATUS "FT-TL: Building ALL x64 SIMD variants")
    add_subdirectory(avx_2)
    add_subdirectory(avx_512)    
elseif(RT_TM_ARCH_ARM64)
    message(STATUS "FT-TL: Building ALL ARM64 SIMD variants")
    add_subdirectory(arm_neon)
    add_subdirectory(arm_sve)
endif()

message(STATUS "FT-TL: CPU variants configured with centralized flag management")
message(STATUS "FT-TL: Runtime CPU detection will select optimal implementation")
