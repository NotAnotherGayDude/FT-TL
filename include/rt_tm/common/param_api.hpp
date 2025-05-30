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
#include <rt_tm/common/model_core.hpp>

namespace rt_tm {

	template<typename derived_type> struct param_api {
		template<uint8_type value_type> RT_TM_FORCE_INLINE value_type get_value(size_t index) {
			value_type* ptr = static_cast<value_type*>(static_cast<derived_type*>(this)->aux_params);
			return ptr[index];
		}

		template<uint16_type value_type> RT_TM_FORCE_INLINE value_type get_value(size_t index) {
			value_type* ptr = static_cast<value_type*>(static_cast<derived_type*>(this)->aux_params);
			return ptr[index];
		}

		template<uint32_type value_type> RT_TM_FORCE_INLINE value_type get_value(size_t index) {
			value_type* ptr = static_cast<value_type*>(static_cast<derived_type*>(this)->aux_params);
			return ptr[index];
		}

		template<uint64_type value_type> RT_TM_FORCE_INLINE value_type get_value(size_t index) {
			value_type* ptr = static_cast<value_type*>(static_cast<derived_type*>(this)->aux_params);
			return ptr[index];
		}

		template<int8_type value_type> RT_TM_FORCE_INLINE value_type get_value(size_t index) {
			value_type* ptr = static_cast<value_type*>(static_cast<derived_type*>(this)->aux_params);
			return ptr[index];
		}

		template<int16_type value_type> RT_TM_FORCE_INLINE value_type get_value(size_t index) {
			value_type* ptr = static_cast<value_type*>(static_cast<derived_type*>(this)->aux_params);
			return ptr[index];
		}

		template<int32_type value_type> RT_TM_FORCE_INLINE value_type get_value(size_t index) {
			value_type* ptr = static_cast<value_type*>(static_cast<derived_type*>(this)->aux_params);
			return ptr[index];
		}

		template<int64_type value_type> RT_TM_FORCE_INLINE value_type get_value(size_t index) {
			value_type* ptr = static_cast<value_type*>(static_cast<derived_type*>(this)->aux_params);
			return ptr[index];
		}

		template<float32_type value_type> RT_TM_FORCE_INLINE value_type get_value(size_t index) {
			value_type* ptr = static_cast<value_type*>(static_cast<derived_type*>(this)->aux_params);
			return ptr[index];
		}

		template<float64_type value_type> RT_TM_FORCE_INLINE value_type get_value(size_t index) {
			value_type* ptr = static_cast<value_type*>(static_cast<derived_type*>(this)->aux_params);
			return ptr[index];
		}
	};

}