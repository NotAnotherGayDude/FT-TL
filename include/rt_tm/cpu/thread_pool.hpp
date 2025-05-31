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

		RT_TM_FORCE_INLINE atomic_wrapper& operator=(atomic_wrapper&& other) {
			if (this != &other) {
				value.store(other.load(std::memory_order_acquire), std::memory_order_release);
			}
			return *this;
		}

		RT_TM_FORCE_INLINE atomic_wrapper(atomic_wrapper&& other) {
			*this = std::move(other);
		}

		RT_TM_FORCE_INLINE void swap(atomic_wrapper& other) noexcept {
			value.store(other.value.load(std::memory_order_acquire), std::memory_order_release);
		}

		std::atomic<value_type> value{};
	};

	struct active_index {
		RT_TM_FORCE_INLINE active_index& operator=(active_index&& other) noexcept {
			if (this != &other) {
				index.value.store(other.index.value.load(std::memory_order_acquire), std::memory_order_release);
			}
			return *this;
		}

		RT_TM_FORCE_INLINE active_index(active_index&& other) noexcept {
			*this = std::move(other);
		}

		RT_TM_FORCE_INLINE void swap(active_index& other) noexcept {
			index.swap(other.index);
		}

		atomic_wrapper<uint64_t> index{};
		std::atomic_flag flag{};
	};

	template<impl_indices indices> struct thread_pool {
		RT_TM_FORCE_INLINE thread_pool(size_t thread_count) {
			worker_latches.resize(thread_count);
			threads.resize(thread_count);
			for (size_t x = 0; x < thread_count; ++x) {
				worker_latches[x] = std::make_unique<std::latch>(1);
				threads[x] = std::thread{ [&,x] {
					thread_function(x);
				} };
			}
		}

		RT_TM_FORCE_INLINE void execute_tasks() {
			main_execution_latch = std::make_unique<std::latch>(threads.size());
			for (auto& value: worker_latches) {
				value->count_down(1);
			}
			//main_execution_latch->wait();
		}

		RT_TM_FORCE_INLINE void thread_function(size_t index) {
			while (!stop.load(std::memory_order_acquire)) {
				worker_latches[index]->wait();
				while (remaining_tasks.load(std::memory_order_acquire) > 0 && !stop.load(std::memory_order_acquire)) {
					worker_latches[index]->wait();
					while (available_tasks.load(std::memory_order_acquire) > 0 && !stop.load(std::memory_order_acquire)) {
						size_t current_index{ index  };
						while (current_index < available_tasks.load(std::memory_order_acquire) && !stop.load(std::memory_order_acquire)) {
							if (!available_indices[current_index].flag.test_and_set()) {
								size_t thread_index{ available_indices[current_index].index.value.load(std::memory_order_acquire) };
								auto& thread{ op_core_threads[thread_index] };
								if (!thread.flag.test_and_set(std::memory_order_acquire)) {
									available_tasks.fetch_sub(1, std::memory_order_release);
									remaining_tasks.fetch_sub(1, std::memory_order_release);
									available_indices[current_index].swap(available_indices[remaining_tasks.load(std::memory_order_acquire) - 1]);
									// Execute the kernel on this thread.
									thread.dependent_op_counter->fetch_add(1, std::memory_order_release);
									if (thread.dependent_op_counter->load() == 0) {
										size_t new_task_count{ thread.dependent_thread_count };
										available_tasks.fetch_add(new_task_count, std::memory_order_release);
										if (new_task_count > 1) {
											for (auto& value: worker_latches) {
												if (!value->try_wait()) {
													--new_task_count;
													value->count_down();
												}
												if (new_task_count == 0) {
													break;
												}
											}
										}
									}
								}
							};

							++current_index;	
						}
					}
					worker_latches[index] = std::make_unique<std::latch>(1);
				}
				if (!main_execution_latch->try_wait()) {
					main_execution_latch->count_down();
				}
			}
		}

		RT_TM_FORCE_INLINE void reset_state(std::vector<op_core>& ops) {
			remaining_tasks.store(op_core_threads.size(), std::memory_order_release);
			for (auto& value: ops) {
				available_tasks.fetch_add(value.depth == 0 ? 1 : 0, std::memory_order_release);
			}
		};

		RT_TM_FORCE_INLINE ~thread_pool() {
			stop.store(true);
			for (auto& value: worker_latches) {
				if (!value->try_wait()) {
					value->count_down(1);
				}
			}
			for (auto& value: threads) {
				if (value.joinable()) {
					value.join();
				}
			}
		}
		
	  protected:
		std::vector<std::unique_ptr<std::latch>> worker_latches{};
		std::unique_ptr<std::latch> main_execution_latch{};
		std::vector<cpu_op_core_thread> op_core_threads{};
		std::vector<active_index> available_indices{};
		std::atomic_uint64_t remaining_tasks{};
		std::atomic_uint64_t available_tasks{};
		std::vector<cpu_op_core> op_cores{};
		std::vector<std::thread> threads{};
		std::atomic_bool stop{};
	};


}