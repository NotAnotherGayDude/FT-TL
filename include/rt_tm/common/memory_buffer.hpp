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

#include <rt_tm/common/allocator.hpp>
#include <rt_tm/common/common.hpp>
#include <rt_tm/common/config.hpp>
#include <stdexcept>
#include <iterator>

namespace rt_tm {

	template<global_config config> struct memory_buffer : public alloc_wrapper<uint8_t> {
		using value_type = uint8_t;
		using alloc		 = alloc_wrapper<value_type>;
		using pointer	 = value_type*;
		using size_type	 = size_t;

		RT_TM_FORCE_INLINE memory_buffer() noexcept = default;

		RT_TM_FORCE_INLINE memory_buffer& operator=(const memory_buffer&) noexcept = delete;
		RT_TM_FORCE_INLINE memory_buffer(const memory_buffer&) noexcept			   = delete;

		RT_TM_FORCE_INLINE memory_buffer& operator=(memory_buffer&&other) noexcept {
			if (this != &other) {
				std::swap(current_offset, other.current_offset);
				std::swap(data_val, other.data_val);
				std::swap(size_val, other.size_val);
			}
			return *this;
		}

		RT_TM_FORCE_INLINE memory_buffer(memory_buffer&& other) noexcept {
			*this = std::move(other);
		}

		RT_TM_FORCE_INLINE void init(size_t size) noexcept {
			data_val = alloc::allocate(size);
			size_val = size;
		}

		RT_TM_FORCE_INLINE pointer claim_memory(size_t amount_to_claim) noexcept {
			if (current_offset + amount_to_claim > size_val) {
				if constexpr (config.exceptions) {
					throw std::runtime_error{ "Sorry, but this memory_buffer is out of memory!" };
				} else {
					return nullptr;
				}
			}
			pointer return_value = data_val + current_offset;
			current_offset += amount_to_claim;
			return return_value;
		}

		RT_TM_FORCE_INLINE ~memory_buffer() noexcept {
			if (data_val && size_val > 0) {
				alloc::deallocate(data_val);
				data_val = nullptr;
			}
		}

	  protected:
		size_type current_offset{};
		value_type* data_val{};
		size_type size_val{};
	};

}
