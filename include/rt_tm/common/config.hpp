/*
MIT License

Copyright (c) 2025 RealTimeChris (Chris M)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "FT-TL Library"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

This file was independently created by RealTimeChris (Chris M), without reuse
or derivation from any codebase owned by other entities, including any contract work.

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT
OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE
OR OTHER DEALINGS IN THE SOFTWARE.
*/
#pragma once

#include <source_location>
#include <cstdint>
#include <utility>
#include <atomic>

#if defined(_MSC_VER)
	#define RT_TM_COMPILER_MSVC 1
	#define RT_TM_INLINE inline
	#define RT_TM_FORCE_INLINE [[msvc::forceinline]] inline
#elif defined(__clang__) || (defined(__GNUC__) && defined(__llvm__))
	#define RT_TM_COMPILER_CLANG 1
	#define RT_TM_INLINE inline
	#define RT_TM_FORCE_INLINE inline __attribute__((always_inline))
#elif defined(__GNUC__) && !defined(__clang__)
	#define RT_TM_COMPILER_GNUCXX 1
	#define RT_TM_INLINE inline
	#define RT_TM_FORCE_INLINE inline __attribute__((always_inline))
#else
	#error "Unsupported compiler"
#endif

#if defined(WIN32) || defined(_WIN32) || defined(_WIN64)
	#define RT_TM_PLATFORM_WINDOWS 1
#elif defined(macintosh) || defined(Macintosh) || (defined(__APPLE__) && defined(__MACH__)) || defined(TARGET_OS_MAC)
	#define RT_TM_PLATFORM_MAC 1
#elif defined(linux) || defined(__linux) || defined(__linux__) || defined(__gnu_linux__)
	#define RT_TM_PLATFORM_LINUX 1
#else
	#error "Unsupported platform"
#endif

#if !defined(RT_TM_LIKELY)
	#define RT_TM_LIKELY(...) (__VA_ARGS__) [[likely]]
#endif

#if !defined(RT_TM_UNLIKELY)
	#define RT_TM_UNLIKELY(...) (__VA_ARGS__) [[unlikely]]
#endif

#if !defined(RT_TM_ELSE_UNLIKELY)
	#define RT_TM_ELSE_UNLIKELY(...) __VA_ARGS__ [[unlikely]]
#endif

#if !defined(RT_TM_ALIGN)
	#define RT_TM_ALIGN(N) alignas(N)
#endif

#if defined(__x86_64__) || defined(_M_X64)
	#define RT_TM_ARCH_X86_64 1
	#include <immintrin.h>
#elif defined(__aarch64__) || defined(_M_ARM64)
	#define RT_TM_ARCH_ARM64 1
#else
	#error "Unsupported architecture"
#endif

#ifndef NDEBUG
	#define RT_TM_ASSERT(x) \
		if (!(x)) \
		rt_tm::internal_abort(#x, std::source_location::current())
#else
	#define RT_TM_ASSERT(x)
#endif
