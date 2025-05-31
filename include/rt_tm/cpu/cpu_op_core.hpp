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

#include <rt_tm/common/op_core.hpp>

namespace rt_tm {

	struct alignas(64) cpu_op_core {
		RT_TM_FORCE_INLINE cpu_op_core(op_core other) {};
		alignas(64) std::atomic_int counter{ -1 };
	};

	struct alignas(64) cpu_op_core_thread {
		alignas(64) std::atomic_int* dependent_op_counter{};
		alignas(64) const size_t dependent_thread_count{};
		alignas(64) const size_t thread_index{};
		alignas(64) const size_t thread_count{};
		alignas(64) std::atomic_flag flag{};
		alignas(64) cpu_op_core* op_core{};
	};

}