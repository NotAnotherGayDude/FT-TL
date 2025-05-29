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

#include <cstdint>

namespace rt_tm {

	enum class data_type : uint32_t {
		float_32 = 0,
		float_16 = 1,
		q8_0	 = 8,
		int_8	 = 24,
		int_16	 = 25,
		int_32	 = 26,
		int_64	 = 27,
		float_64 = 28,
		count,
	};

	enum class op_type : uint32_t {
		unset	 = 0,
		noop	 = 1,
		mul_mat	 = 2,
		add		 = 3,
		sub		 = 4,
		int_16	 = 5,
		int_32	 = 6,
		int_64	 = 7,
		float_64 = 8,
		count,
	};

	enum class device_type {
		cpu = 0,
		gpu = 1,
	};

	struct global_config {
		bool exceptions{};
	};

	struct cli_params {
		std::string model_file{};
		size_t thread_count{};
	};

	struct impl_indices {
		size_t cpu_index{};
		size_t gpu_index{};
	};

}
