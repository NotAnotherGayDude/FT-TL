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

#include <rt_tm/op_graph.hpp>
#include <rt_tm/common/model_parser.hpp>
#include <rt_tm/common/common.hpp>
#include <cstdint>

namespace rt_tm {

	template<global_config config> struct harbinger {
		template<model_format format> RT_TM_FORCE_INLINE static model_graph<config> parse_model_graph(std::string_view path) {
			return model_parser<config, format>::parse_model(path);
		}

		RT_TM_FORCE_INLINE static op_graph<config> create_op_graph(op_graph_config graph_config, const model_graph<config>&) {
			return op_graph<config>{ graph_config };
		}

		RT_TM_FORCE_INLINE static cli_params parse_cli_arguments(const std::string& command_line) {
			cli_params result{};
			std::istringstream stream(command_line);
			std::string current_flag{};
			std::string token{};
			bool expect_value = false;

			while (stream >> std::quoted(token) || stream >> token) {
				if (token.empty())
					continue;

				if (token[0] == '-') {
					current_flag = token;
					if (token == "-m" || token == "-t") {
						expect_value = true;
					} else {
						expect_value = false;
					}
				} else if (expect_value) {
					if (current_flag == "-m") {
						result.model_file = token;
					} else if (current_flag == "-t") {
						try {
							result.thread_count = std::stoull(token);
						} catch (const std::exception&) {
							result.thread_count = 1;
						}
					}
					expect_value = false;
				}
			}

			return result;
		}
	};

}
