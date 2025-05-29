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

cmake_minimum_required(VERSION 3.28)

set(PROJECT_NAME "rt_tm")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(ENV{CMAKE_INSTALL_PREFIX} "${CMAKE_SOURCE_DIR}/../Install")

project(
    "${PROJECT_NAME}"
    VERSION "1.0.0"
    DESCRIPTION "FT-TL: FakeTime Tensor Language - You trained the model. FT-TL gives it a voice."
    LANGUAGES CXX
)

if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
    set(RT_TM_ARCH_X64 TRUE)
    set(RT_TM_BUILD_ALL_X64_VARIANTS TRUE)
    message(STATUS "FT-TL: Building ALL x64 SIMD variants for runtime selection")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|ARM64")
    set(RT_TM_ARCH_ARM64 TRUE)
    set(RT_TM_BUILD_ALL_ARM_VARIANTS TRUE)
    message(STATUS "FT-TL: Building ALL ARM64 SIMD variants for runtime selection")
endif()

message(STATUS "FT-TL: Detected architecture: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "FT-TL: Universal binary strategy: Runtime CPU detection")

add_library("${PROJECT_NAME}" INTERFACE)
add_library("${PROJECT_NAME}::${PROJECT_NAME}" ALIAS "${PROJECT_NAME}")

file(GLOB_RECURSE RT_TM_HEADERS CONFIGURE_DEPENDS 
    "${CMAKE_CURRENT_SOURCE_DIR}/include/rt_tm/*.hpp"
)

set_target_properties(
    "${PROJECT_NAME}" PROPERTIES
    OUTPUT_NAME "rt_tm"
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    INTERFACE_COMPILE_FEATURES cxx_std_23
)

target_include_directories(
    "${PROJECT_NAME}" INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>"
    "$<INSTALL_INTERFACE:include/>"
)

target_compile_options(
    "${PROJECT_NAME}" INTERFACE
    "$<$<CXX_COMPILER_ID:MSVC>:/Zc:preprocessor>"
    "$<$<CXX_COMPILER_ID:MSVC>:/permissive->"
    "$<$<CXX_COMPILER_ID:MSVC>:/Zc:lambda>"
    "$<$<CXX_COMPILER_ID:MSVC>:/bigobj>"
    "$<$<CONFIG:RELEASE>:$<$<CXX_COMPILER_ID:MSVC>:/GL>>"
    "$<$<CONFIG:RELEASE>:$<$<CXX_COMPILER_ID:MSVC>:/Gy>>"
    "$<$<CONFIG:RELEASE>:$<$<CXX_COMPILER_ID:MSVC>:/O2>>"
    "$<$<CONFIG:RELEASE>:$<$<CXX_COMPILER_ID:MSVC>:/Ob>>"
    "$<$<CONFIG:RELEASE>:$<$<CXX_COMPILER_ID:MSVC>:/fp:fast>>"
    "$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-O3>"
    "$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-ffast-math>"
    "$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-funroll-loops>"
    "$<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-fprefetch-loop-arrays>"
    "$<$<AND:$<STREQUAL:$<UPPER_CASE:${UBSAN_ENABLED}>,TRUE>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-fsanitize=undefined>"
    "$<$<CXX_COMPILER_ID:MSVC>:$<$<STREQUAL:${ASAN_ENABLED},TRUE>:/fsanitize=address>>"
)

target_link_options(
    "${PROJECT_NAME}" INTERFACE
    "$<$<CXX_COMPILER_ID:MSVC>:/INCREMENTAL:NO>"
    "$<$<CXX_COMPILER_ID:MSVC>:/OPT:ICF>"
    "$<$<CXX_COMPILER_ID:MSVC>:/OPT:REF>"
    "$<$<CXX_COMPILER_ID:MSVC>:/LTCG>"
    "$<$<CXX_COMPILER_ID:Clang>:-flto=thin>"
    "$<$<CXX_COMPILER_ID:GNU>:-flto>"
    "$<$<CXX_COMPILER_ID:Clang>:-Wl,-O3>"
    "$<$<CXX_COMPILER_ID:GNU>:-Wl,-O3>"
    "$<$<AND:$<STREQUAL:$<UPPER_CASE:${UBSAN_ENABLED}>,TRUE>,$<NOT:$<CXX_COMPILER_ID:MSVC>>>:-fsanitize=undefined>"
    "$<$<AND:$<CXX_COMPILER_ID:Clang>,$<STREQUAL:${ASAN_ENABLED},TRUE>>:-fsanitize=address>"
    "$<$<AND:$<CXX_COMPILER_ID:GNU>,$<STREQUAL:${ASAN_ENABLED},TRUE>>:-fsanitize=address>"
)

target_compile_definitions(
    "${PROJECT_NAME}" INTERFACE 
    "$<$<STREQUAL:${ASAN_ENABLED},TRUE>:ASAN_ENABLED>"
)

add_subdirectory(source/rt_tm/cpu)

if(RT_TM_ARCH_X64)
    target_link_libraries("${PROJECT_NAME}" INTERFACE rt_tm::avx2 rt_tm::avx512)
    message(STATUS "FT-TL: Main library linked with ALL x64 variants")
elseif(RT_TM_ARCH_ARM64)
    target_link_libraries("${PROJECT_NAME}" INTERFACE rt_tm::arm_neon rt_tm::arm_sve)
    message(STATUS "FT-TL: Main library linked with ALL ARM64 variants")
endif()

set(CONFIG_FILE_NAME "${PROJECT_NAME}Config.cmake")
set(EXPORTED_TARGETS_NAME "${PROJECT_NAME}Targets")
set(EXPORTED_TARGETS_FILE_NAME "${EXPORTED_TARGETS_NAME}.cmake")
set(EXPORTED_TARGETS_FILE_PATH "share/rt_tm/${EXPORTED_TARGETS_FILE_NAME}")

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_SOURCE_DIR}/cmake/${CONFIG_FILE_NAME}.in"
    "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_FILE_NAME}"
    INSTALL_DESTINATION "share/rt_tm"
    PATH_VARS EXPORTED_TARGETS_FILE_PATH
)

set(VERSION_FILE_NAME "${PROJECT_NAME}ConfigVersion.cmake")
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${VERSION_FILE_NAME}"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY AnyNewerVersion
)

install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${CONFIG_FILE_NAME}"
    "${CMAKE_CURRENT_BINARY_DIR}/${VERSION_FILE_NAME}"
    DESTINATION "share/rt_tm"
)

install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/"
    DESTINATION "include"
    FILES_MATCHING PATTERN "*.hpp"
)

set(RT_TM_ALL_TARGETS "${PROJECT_NAME}")
if(RT_TM_ARCH_X64)
    list(APPEND RT_TM_ALL_TARGETS rt_tm_avx2 rt_tm_avx512)
elseif(RT_TM_ARCH_ARM64)
    list(APPEND RT_TM_ALL_TARGETS rt_tm_arm_neon rt_tm_arm_sve)
endif()

install(
    TARGETS ${RT_TM_ALL_TARGETS}
    EXPORT "${EXPORTED_TARGETS_NAME}"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
    RUNTIME DESTINATION "bin"
)

install(
    EXPORT "${EXPORTED_TARGETS_NAME}"
    FILE "${EXPORTED_TARGETS_FILE_NAME}"
    NAMESPACE "${PROJECT_NAME}::"
    DESTINATION "share/rt_tm"
)

message(STATUS "")
message(STATUS "FT-TL Configuration Summary:")
message(STATUS "==========================")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Architecture: ${CMAKE_SYSTEM_PROCESSOR}")
message(STATUS "Strategy: Universal Binary with Runtime CPU Detection")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
if(RT_TM_BUILD_ALL_X64_VARIANTS)
    message(STATUS "x64 Variants: SSE4.2, AVX2, AVX-512 (All built)")
endif()
if(RT_TM_BUILD_ALL_ARM_VARIANTS)
    message(STATUS "ARM Variants: NEON, SVE (All built)")
endif()
message(STATUS "")
message(STATUS "FT-TL: FakeTime Tensor Language")
message(STATUS "The fastest tensor computation library on Earth")
message(STATUS "Universal binaries with runtime CPU detection")
message(STATUS "")

if (RT_TM_VS_LLAMA)
    add_subdirectory("./tests/vs-llama")
endif()