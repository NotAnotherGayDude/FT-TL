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

#include <rt_tm/common/common.hpp>
#include <rt_tm/cpu/thread_pool.hpp>

namespace rt_tm {

	template<device_type dev, global_config config, impl_indices indices> struct device;

	template<device_type dev, global_config config, impl_indices indices> struct device_registry;

	template<global_config config, impl_indices indices> struct device<device_type::cpu, config, indices> : public cpu_scheduler<device<device_type::cpu, config, indices>> {
		using thread_pool_t	  = thread_pool<indices>;
		using memory_buffer_t = memory_buffer<config>;

		thread_pool_t thread_pool_val{};
		memory_buffer_t tensor_buffer{};
		memory_buffer_t scratch_buffer{};
		memory_buffer_t param_buffer{};

		RT_TM_FORCE_INLINE device(size_t thread_count) : thread_pool_val{ thread_count } {};

		RT_TM_FORCE_INLINE void execute_tasks() {
			thread_pool_val.execute_tasks();
		}

		RT_TM_FORCE_INLINE void reset_state(std::vector<op_core>& ops) {
			cpu_scheduler<device<device_type::cpu, config, indices>>::schedule_tasks(ops);
		}
	};

	template<global_config config, impl_indices indices> struct device_registry<device_type::cpu, config, indices> {
		RT_TM_FORCE_INLINE device_registry(size_t thread_count) {
			devices.emplace_back(std::make_unique<device<device_type::cpu, config, indices>>(thread_count));
		}

		RT_TM_FORCE_INLINE auto& get_devices() {
			return devices;
		}

		std::vector<std::unique_ptr<device<device_type::cpu, config, indices>>> devices{};
	};

}