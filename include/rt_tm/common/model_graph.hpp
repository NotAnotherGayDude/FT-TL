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

#include <rt_tm/common/model_core.hpp>
#include <rt_tm/common/memory_buffer.hpp>
#include <rt_tm/common/common.hpp>

namespace rt_tm {

	struct tokenizer_parameters {
		std::vector<int64_t> token_types{};
		std::vector<std::string> tokens{};
		std::vector<std::string> merges{};
		std::string chat_template{};
		uint64_t bos_token_id{};
		uint64_t eos_token_id{};
		std::string pre{};
	};

	struct hyper_parameters {
		uint64_t imatrix_entries_count{};
		uint64_t imatrix_chunks_count{};
		uint64_t quantization_version{};
		uint64_t rope_dimension_count{};
		uint64_t feed_forward_length{};
		uint64_t embedding_length{};
		std::string imatrix_file{};
		uint64_t context_length{};
		uint64_t head_count_kv{};
		float rms_norm_epsilon{};
		uint64_t block_count{};
		float rope_freq_base{};
		uint64_t head_count{};
		uint64_t vocab_size{};
		uint64_t file_type{};
	};

	template<global_config config> struct model_graph {
		tokenizer_parameters tokenizer_params{};
		std::vector<model_core> model_cores{};
		memory_buffer<config> tensor_data{};
		hyper_parameters hparams{};
	};

}