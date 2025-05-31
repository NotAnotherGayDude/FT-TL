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
#include <rt_tm/common/op_core.hpp>
#include <rt_tm/common/memory_buffer.hpp>
#include <rt_tm/cpu/device.hpp>

namespace rt_tm {

	struct op_graph_base_low {
		virtual ~op_graph_base_low() {
		}
	};

	template<global_config config, impl_indices indices_new> struct op_graph_base : public op_graph_base_low, public device_registry<device_type::cpu, config, indices_new> {
	  public:
		inline static constexpr impl_indices indices{ indices_new };
		op_graph_config config_val{};

		RT_TM_FORCE_INLINE op_graph_base(op_graph_config graph_config)
			: device_registry<device_type::cpu, config, indices_new>{ graph_config.num_threads }, config_val{ graph_config } {};

		RT_TM_FORCE_INLINE void reset_state(size_t dev_index, device_type dev_type) {
			for (auto& device: this->get_devices()) {
				device->reset_state(ops);
			}
		}

		RT_TM_FORCE_INLINE void execute_tasks(size_t dev_index, device_type dev_type) {
			for (auto& device: this->get_devices()) {
				device->execute_tasks();
			}
		}

		RT_TM_FORCE_INLINE ~op_graph_base() {
		}

	  protected:
		std::vector<op_core> ops{};
	};

	template<global_config config> struct op_graph {
		RT_TM_FORCE_INLINE op_graph& operator=(op_graph&& other) {
			this->op_graph_val.swap(other.op_graph_val);
			return *this;
		}

		RT_TM_FORCE_INLINE op_graph(op_graph&& other) {
			*this = std::move(other);
		}

		op_graph(op_graph_config graph_config)
			: op_graph_val{ [&] {
				  std::unique_ptr<op_graph_base_low> return_Value{};
				  switch (cpu_arch_index_holder::cpu_arch_index) {
					  case 0: {
						  return_Value = std::make_unique<op_graph_base<config, impl_indices{ .cpu_index = 0 }>>(graph_config);
						  break;
					  }
					  case 1: {
						  return_Value = std::make_unique<op_graph_base<config, impl_indices{ .cpu_index = 1 }>>(graph_config);
						  break;
					  }
					  case 2: {
						  return_Value = std::make_unique<op_graph_base<config, impl_indices{ .cpu_index = 2 }>>(graph_config);
						  break;
					  }
				  }
				  return return_Value;
			  }() } {
		}

		RT_TM_FORCE_INLINE void reset_state(size_t dev_index, device_type dev_type) {
			switch (cpu_arch_index_holder::cpu_arch_index) {
				case 0: {
					static_cast<op_graph_base<config, impl_indices{ .cpu_index = 0 }>*>(op_graph_val.get())->reset_state(dev_index, dev_type);
					break;
				}
				case 1: {
					static_cast<op_graph_base<config, impl_indices{ .cpu_index = 1 }>*>(op_graph_val.get())->reset_state(dev_index, dev_type);
					break;
				}
				case 2: {
					static_cast<op_graph_base<config, impl_indices{ .cpu_index = 2 }>*>(op_graph_val.get())->reset_state(dev_index, dev_type);
					break;
				}
			}
		}

		RT_TM_FORCE_INLINE void execute_tasks(size_t dev_index, device_type dev_type) {
			switch (cpu_arch_index_holder::cpu_arch_index) {
				case 0: {
					static_cast<op_graph_base<config, impl_indices{ .cpu_index = 0 }>*>(op_graph_val.get())->execute_tasks(dev_index, dev_type);
					break;
				}
				case 1: {
					static_cast<op_graph_base<config, impl_indices{ .cpu_index = 1 }>*>(op_graph_val.get())->execute_tasks(dev_index, dev_type);
					break;
				}
				case 2: {
					static_cast<op_graph_base<config, impl_indices{ .cpu_index = 2 }>*>(op_graph_val.get())->execute_tasks(dev_index, dev_type);
					break;
				}
			}
		}

		RT_TM_FORCE_INLINE void init(op_graph_config graph_config = {}) {
			op_graph_val = [&] {
				std::unique_ptr<op_graph_base_low> return_Value{};
				switch (cpu_arch_index_holder::cpu_arch_index) {
					case 0: {
						return_Value = std::make_unique<op_graph_base<config, impl_indices{ .cpu_index = 0 }>>(graph_config);
						break;
					}
					case 1: {
						return_Value = std::make_unique<op_graph_base<config, impl_indices{ .cpu_index = 1 }>>(graph_config);
						break;
					}
					case 2: {
						return_Value = std::make_unique<op_graph_base<config, impl_indices{ .cpu_index = 2 }>>(graph_config);
						break;
					}
				}
				return return_Value;
			}();
		}

	  protected:
		std::unique_ptr<op_graph_base_low> op_graph_val{};
	};

}