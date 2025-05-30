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

#include <type_traits>
#include <concepts>

namespace rt_tm {

	template<typename value_type>
	concept uint_type = std::is_unsigned_v<std::remove_cvref_t<value_type>> && std::is_integral_v<std::remove_cvref_t<value_type>>;

	template<typename value_type>
	concept int_type = std::is_signed_v<std::remove_cvref_t<value_type>> && std::is_integral_v<std::remove_cvref_t<value_type>>;

	template<typename value_type>
	concept int8_type = int_type<std::remove_cvref_t<value_type>> && sizeof(value_type) == 1;

	template<typename value_type>
	concept int16_type = int_type<std::remove_cvref_t<value_type>> && sizeof(value_type) == 2;

	template<typename value_type>
	concept int32_type = int_type<std::remove_cvref_t<value_type>> && sizeof(value_type) == 4;

	template<typename value_type>
	concept int64_type = int_type<std::remove_cvref_t<value_type>> && sizeof(value_type) == 8;

	template<typename value_type>
	concept uint8_type = uint_type<std::remove_cvref_t<value_type>> && sizeof(value_type) == 1;

	template<typename value_type>
	concept uint16_type = uint_type<std::remove_cvref_t<value_type>> && sizeof(value_type) == 2;

	template<typename value_type>
	concept uint32_type = uint_type<std::remove_cvref_t<value_type>> && sizeof(value_type) == 4;

	template<typename value_type>
	concept uint64_type = uint_type<std::remove_cvref_t<value_type>> && sizeof(value_type) == 8;

	template<typename value_type>
	concept float_type = std::floating_point<std::remove_cvref_t<value_type>>;

	template<typename value_type>
	concept float32_type = float_type<value_type> && sizeof(value_type) == 4;

	template<typename value_type>
	concept float64_type = float_type<value_type> && sizeof(value_type) == 8;

}
