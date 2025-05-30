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

#include <rt_tm/cpu/cpu_scheduler.hpp>
#include <rt_tm/common/common.hpp>
#include <rt_tm/common/tuple.hpp>
#include <rt_tm/cpu/cpu_op_core.hpp>
#include <condition_variable>
#include <thread>
#include <latch>

namespace rt_tm {

	template<typename value_type> struct alignas(64) atomic_wrapper {
		RT_TM_FORCE_INLINE atomic_wrapper() = default;
		RT_TM_FORCE_INLINE atomic_wrapper& operator=(const atomic_wrapper& other) {
			return *this;
		};
		RT_TM_FORCE_INLINE atomic_wrapper(const atomic_wrapper& other) {
			*this = other;
		}
		std::atomic<value_type> value{};
	};

	template<impl_indices indices> struct worker_thread {};

	template<impl_indices indices> struct cpu_thread_task {
		std::atomic_uint64_t* depentent_op_index{};
		cpu_op_core_thread<0>* task{};
		std::atomic_int64_t* dependent_op{};
		std::atomic_flag* flag{};
	};

	template<impl_indices indices> struct thread_pool {
		RT_TM_FORCE_INLINE thread_pool(size_t thread_count) {};
		RT_TM_FORCE_INLINE void execute_tasks() {
		}
		RT_TM_FORCE_INLINE void reset_state() {
		}
	  protected:
		std::vector<std::unique_ptr<std::latch>> worker_latches{};
		std::unique_ptr<std::latch> main_execution_latch{};
		std::vector<atomic_wrapper<bool>> working_val{};
		std::vector<std::thread> threads{};
		std::atomic_bool stop{};
	};


}