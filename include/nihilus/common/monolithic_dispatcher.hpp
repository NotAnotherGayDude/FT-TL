/*
Copyright (c) 2025 RealTimeChris (Chris M.)

This file is part of software offered under a restricted-use license to a designated Licensee,
whose identity is confirmed in writing by the Author.

License Terms (Summary):
- Exclusive, non-transferable license for internal use only.
- Redistribution, sublicensing, or public disclosure is prohibited without written consent.
- Full ownership remains with the Author.
- License may terminate if unused for [X months], if materially breached, or by mutual agreement.
- No warranty is provided, express or implied.

Full license terms are provided in the LICENSE file distributed with this software.

Signed,
RealTimeChris (Chris M.)
2025
*/

#pragma once

#include <nihilus/common/kernel_traits.hpp>
#include <nihilus/common/common.hpp>
#include <nihilus/common/core_traits.hpp>
#include <nihilus/cpu/cpu_arch.hpp>

namespace nihilus {

	array<size_t, 22> depths_new{};

	template<model_config config, device_type dev_type, kernel_type type, single_input core_type> struct kernel_dispatcher
		: public kernel_traits<type, core_type, typename core_type::input_type01> {
		NIHILUS_FORCE_INLINE static void impl(core_type& params) {
			kernel_dispatcher_impl<cpu_arch_index, type, typename core_type::transform_type, typename core_type::output_type, typename core_type::input_type01::output_type>::impl(
				params.count, params.data, get_adjacent_value<config, core_type::type, 0>::impl(params).data);
			++depths_new[core_type::depth];
		}
	};

	template<model_config config, device_type dev_type, kernel_type type, double_input core_type> struct kernel_dispatcher<config, dev_type, type, core_type>
		: public kernel_traits<type, core_type, typename core_type::input_type01, typename core_type::input_type02> {
		NIHILUS_FORCE_INLINE static void impl(core_type& params) {
			kernel_dispatcher_impl<cpu_arch_index, type, typename core_type::transform_type, typename core_type::output_type, typename core_type::input_type01::output_type,
				typename core_type::input_type02::output_type>::impl(params.count, params.data, get_adjacent_value<config, core_type::type, 0>::impl(params).data,
				get_adjacent_value<config, core_type::type, 1>::impl(params).data);
			++depths_new[core_type::depth];
		}
	};

	template<model_config config, device_type dev_type, kernel_type type, triple_input core_type> struct kernel_dispatcher<config, dev_type, type, core_type>
		: public kernel_traits<type, core_type, typename core_type::input_type01, typename core_type::input_type02, typename core_type::input_type03> {
		NIHILUS_FORCE_INLINE static void impl(core_type& params) {
			kernel_dispatcher_impl<cpu_arch_index, type, typename core_type::transform_type, typename core_type::output_type, typename core_type::input_type01::output_type,
				typename core_type::input_type02::output_type, typename core_type::input_type03::output_type>::impl(params.count, params.data,
				get_adjacent_value<config, core_type::type, 0>::impl(params).data, get_adjacent_value<config, core_type::type, 1>::impl(params).data,
				get_adjacent_value<config, core_type::type, 2>::impl(params).data);
			++depths_new[core_type::depth];
		}
	};

}