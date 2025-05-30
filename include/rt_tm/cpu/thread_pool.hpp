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
		RT_TM_FORCE_INLINE atomic_wrapper()						 = default;
		RT_TM_FORCE_INLINE atomic_wrapper& operator=(const atomic_wrapper& other) {
			return *this;
		};
		RT_TM_FORCE_INLINE atomic_wrapper(const atomic_wrapper& other) {
			*this = other;
		}
		std::atomic<value_type> value{};
	};

	template<impl_indices indices> struct cas_queue {
	  protected:
		std::vector<std::vector<cpu_op_core_thread>> task_threads{};
		std::atomic_uint64_t lowest_index{};
		std::vector<cpu_op_core> tasks{};
	};

	template<impl_indices indices> struct thread_pool : public cas_queue<indices> {
		RT_TM_FORCE_INLINE thread_pool() noexcept = default;
		RT_TM_FORCE_INLINE thread_pool(size_t thread_count) {
			working_val.resize(thread_count);
			for (size_t x = 0; x < thread_count; ++x) {
				threads.emplace_back(std::jthread{ [&, x] {
					thread_function(x);
				} });
			}
		}

		RT_TM_FORCE_INLINE void execute_tasks() {
		}

		RT_TM_FORCE_INLINE bool all_working() noexcept {
			for (auto& value: working_val) {
				if (!value.load(std::memory_order_acquire)) {
					return false;
				}
			}
			return true;
		}

		RT_TM_FORCE_INLINE bool any_working() noexcept {
			for (auto& value: working_val) {
				if (value.value.load(std::memory_order_acquire)) {
					return true;
				}
			}
			return false;
		}

		template<typename cpu_scheduler_type> RT_TM_FORCE_INLINE bool reset_state(std::vector<op_core>& ops, cpu_scheduler_type& cpu_scheduler) {
			cpu_scheduler.template schedule_tasks(*this, ops);
		}

		RT_TM_FORCE_INLINE void thread_function(size_t current_index) {
			while (!stop.load(std::memory_order_acquire)) {
				latch->wait();
				working_val[current_index].value.store(true, std::memory_order_release);
				while (this->lowest_index.load(std::memory_order_acquire) < this->tasks.size() && !stop.load(std::memory_order_acquire)) {
					size_t local_index{ this->lowest_index.load(std::memory_order_acquire) };
					while (local_index < this->tasks.size()) {
						cpu_op_core& op{ this->tasks[local_index] };
						if (op.flag.load(std::memory_order_acquire) == 0) {
							int64_t local_index_thread = op.flag.fetch_add(1, std::memory_order_acq_rel);
							if (local_index_thread < 0) {
							} else if (local_index_thread < op.thread_count) {
								cpu_op_core_thread& thread = this->task_threads[local_index][local_index_thread];
								if (thread.flag.test_and_set()) {
									// execute
									thread.flag.clear();
									thread.op_core->dependent_op->fetch_add(1, std::memory_order_release);
								}
							} else {
								this->lowest_index.fetch_add(1, std::memory_order_release);
							}

						}

						if (op.flag.load(std::memory_order_acquire) >= op.thread_count) {
							this->lowest_index.fetch_add(1, std::memory_order_release);
						}
						++local_index;
					}
				}
				working_val[current_index].value.store(false, std::memory_order_release);
				while (working_val_main.value.load(std::memory_order_acquire)) {
				}
				latch->arrive_and_wait();
			}
		};

		~thread_pool() noexcept {
			stop.store(true, std::memory_order_release);
		}

	  protected:
		std::vector<atomic_wrapper<bool>> working_val{};
		atomic_wrapper<bool> working_val_main{};
		std::unique_ptr<std::latch> latch{};
		std::vector<std::jthread> threads{};
		std::atomic_bool stop{};
	};

}