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

#include <nihilus/common/tokenizer.hpp>
#include <nihilus/common/config.hpp>
#include <iterator>

namespace nihilus {

	struct input_session_config {
		NIHILUS_FORCE_INLINE input_session_config& operator=(const input_session_config&) = delete;
		NIHILUS_FORCE_INLINE input_session_config(const input_session_config&)			= delete;
		NIHILUS_FORCE_INLINE input_session_config(std::istream& stream_new, uint64_t max_tokens_new) : stream{ stream_new }, max_tokens{ max_tokens_new } {};
		std::istream& stream;
		uint64_t max_tokens{};
	};

	template<typename model_type> struct input_session : public tokenizer<model_type::model_traits_type::arch> {
		NIHILUS_FORCE_INLINE input_session(const input_session_config&, model_type& model) : model_ptr{ &model } {};

		NIHILUS_FORCE_INLINE bool process_input() {
			this->tokenize(input, model_ptr->template get_core<model_type::op_type_type::inp_tokens>().data);
			model_ptr->execute_model(exec_params);
			return false;
		}

		NIHILUS_FORCE_INLINE operator bool() {
			return false;
		}

		execution_parameters exec_params{};

	  protected:
		model_type* model_ptr{};
		std::string input{};
	};

}
