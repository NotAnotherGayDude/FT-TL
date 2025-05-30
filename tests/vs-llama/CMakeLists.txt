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

cmake_minimum_required(VERSION 3.18)

project(
  "rt_tm_performance"
  VERSION "${PRODUCT_VERSION}"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include(FetchContent)

FetchContent_Declare(
  llama.cpp
  GIT_REPOSITORY https://github.com/realtimechris/llama.cpp.git
  GIT_TAG master
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(llama.cpp)

FetchContent_Declare(
  benchmarksuite
  GIT_REPOSITORY https://github.com/realtimechris/benchmarksuite.git
  GIT_TAG main
  GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(benchmarksuite)

add_executable(
  "rt_tm_performance" 
  "./main.cpp"
 "save_tensor.h")

add_subdirectory("./common")

target_link_libraries(
	"rt_tm_performance" PUBLIC 
	rt_tm::rt_tm llama common BenchmarkSuite::BenchmarkSuite
)

target_compile_options(
	"rt_tm_performance" PUBLIC
	"$<$<CXX_COMPILER_ID:MSVC>:$<$<STREQUAL:${ASAN_ENABLED},TRUE>:/fsanitize=address>>"
	"$<$<CXX_COMPILER_ID:CLANG>:-Wnull-dereference>"
	"$<$<CXX_COMPILER_ID:CLANG>:-Wuninitialized>"
	"$<$<CXX_COMPILER_ID:CLANG>:-Wconversion>"
	"$<$<CXX_COMPILER_ID:CLANG>:-Wpedantic>"
	"$<$<CXX_COMPILER_ID:CLANG>:-Wshadow>"
	"$<$<CXX_COMPILER_ID:CLANG>:-Wextra>"
	"$<$<CXX_COMPILER_ID:CLANG>:-Wall>"
	
	"$<$<CXX_COMPILER_ID:GNU>:-Wnull-dereference>"
	"$<$<CXX_COMPILER_ID:GNU>:-Wuninitialized>"
	"$<$<CXX_COMPILER_ID:GNU>:-Wconversion>"
	"$<$<CXX_COMPILER_ID:GNU>:-Wpedantic>"
	"$<$<CXX_COMPILER_ID:GNU>:-Wshadow>"
	"$<$<CXX_COMPILER_ID:GNU>:-Wextra>"
	"$<$<CXX_COMPILER_ID:GNU>:-Wall>"

	"$<$<CXX_COMPILER_ID:MSVC>:/Wall>"
	"$<$<CXX_COMPILER_ID:MSVC>:/W4>"
)

target_link_options(
	"rt_tm_performance" PUBLIC
	"$<$<CXX_COMPILER_ID:GNU>:$<$<STREQUAL:${ASAN_ENABLED},TRUE>:-fsanitize=address>>"
)

if (WIN32)
	install(
		FILES 
		"$<TARGET_PDB_FILE:rt_tm_performance>"
		DESTINATION "bin"
		OPTIONAL
	)
endif()

install(
	FILES 
	"$<TARGET_FILE:rt_tm_performance>"
	DESTINATION "bin"
	OPTIONAL
)

install(
      FILES "$<TARGET_FILE:ggml>"
      DESTINATION "$<TARGET_FILE_DIR:rt_tm_performance>"
      OPTIONAL
    )
    install(
      FILES "$<TARGET_FILE:ggml-cpu>"
      DESTINATION "$<TARGET_FILE_DIR:rt_tm_performance>"
      OPTIONAL
    )
    install(
      FILES "$<TARGET_FILE:ggml-base>"
      DESTINATION "$<TARGET_FILE_DIR:rt_tm_performance>"
      OPTIONAL
    )

install(
	FILES 
	"$<TARGET_FILE:llama>"
	DESTINATION "$<TARGET_FILE_DIR:rt_tm_performance>"
	OPTIONAL
)
