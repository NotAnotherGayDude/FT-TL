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
#include <latch>

namespace rt_tm {

	template<impl_indices indices, size_t depth_new> struct cas_queue {
	  protected:
		std::vector<cpu_op_core<depth_new>*> task_ptrs{};
		std::vector<cpu_op_core<depth_new>> tasks{};
	};

	template<impl_indices indices, size_t depth_new> struct scheduler_depth : public cas_queue<indices, depth_new> {
		static constexpr size_t depth{ depth_new };

		template<typename cpu_scheduler_type> RT_TM_FORCE_INLINE bool reset_state(std::vector<op_core>& ops, cpu_scheduler_type& cpu_scheduler, size_t max_depth) {
			if (depth < max_depth) {
				std::cout << "WERE HERE THIS IS IT!" << std::endl;
				cpu_scheduler.template schedule_tasks<depth_new>(*this, ops);
				latch = std::make_unique<std::latch>(this->tasks.size());
				return true;
			} else {
				std::cout << "WERE HERE THIS IS NOT IT!" << std::endl;
				return false;
			}
		}

		RT_TM_FORCE_INLINE bool thread_function(size_t max_depth) {
			if (depth < max_depth) {
				return true;
				latch->arrive_and_wait();
			} else {
				return false;
			}
		};

	  protected:
		std::unique_ptr<std::latch> latch{};
	};

	template<typename... bases> struct scheduler_depths : public bases... {
		template<typename op_entity_type, typename... arg_types> RT_TM_FORCE_INLINE bool iterate_values_impl(arg_types&&... args) {
			return op_entity_type::thread_function(std::forward<arg_types>(args)...);
		}

		template<typename... arg_types> constexpr void iterate_values(arg_types&&... args) {
			(iterate_values_impl<bases>(args...) && ...);
		}

		template<typename op_entity_type, typename... arg_types> RT_TM_FORCE_INLINE bool reset_state_impl(arg_types&&... args) {
			return op_entity_type::reset_state(std::forward<arg_types>(args)...);
		}

		template<typename... arg_types> constexpr void reset_state(arg_types&&... args) {
			(reset_state_impl<bases>(args...) && ...);
		}
	};

	template<impl_indices indices, typename index_sequence> struct get_scheduler_base;

	template<impl_indices indices, size_t... index> struct get_scheduler_base<indices, std::index_sequence<index...>> {
		using type = scheduler_depths<scheduler_depth<indices,index>...>;
	};

	template<impl_indices indices>using scheduler_base_t = typename get_scheduler_base<indices,std::make_index_sequence<32>>::type;

	template<impl_indices indices> struct thread_pool {
		scheduler_base_t<indices> scheduler_depths{};

		template<typename cpu_scheduler_type> RT_TM_FORCE_INLINE void reset_state(std::vector<op_core>& ops, cpu_scheduler_type& cpu_scheduler) {
			scheduler_depths.reset_state(ops, cpu_scheduler, 12);
		};

		RT_TM_FORCE_INLINE void execute_tasks() {
			scheduler_depths.iterate_values(12);
		}
	};

}