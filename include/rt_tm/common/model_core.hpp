/*
MIT License

Copyright (c) 2025 RealTimeChris (Chris M)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "RT-TM Library"), to deal
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

#include <rt_tm/common/type_traits.hpp>
#include <rt_tm/common/allocator.hpp>
#include <rt_tm/common/common.hpp>

namespace rt_tm {

	struct model_core {
		std::vector<uint8_t, alloc_wrapper<uint8_t>> data{};
		array<size_t, 4> dims{ 1, 1, 1, 1 };
		std::string name{};
		data_type type{};

		RT_TM_FORCE_INLINE size_t core_total_dims() const {
			return dims[0] * dims[1] * dims[2] * dims[3];
		}

		RT_TM_FORCE_INLINE size_t core_total_byte_size() const {
			size_t total_elements = core_total_dims();
			size_t block_size	  = core_block_size();
			size_t type_size	  = core_type_size();
			size_t num_blocks = (total_elements + block_size - 1) / block_size;
			return num_blocks * type_size;
		}

		RT_TM_INLINE size_t core_block_size() const {
			return get_type_traits(type).block_size;
		}

		RT_TM_INLINE size_t core_type_size() const {
			return get_type_traits(type).type_size;
		}

		RT_TM_INLINE size_t core_row_size(int64_t dims_new) const {
			return core_type_size() * dims_new / core_block_size();
		}
	};

}