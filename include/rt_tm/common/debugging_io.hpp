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

#include <rt_tm/common/op_core.hpp>
#include <rt_tm/common/common.hpp>
#include <filesystem>
#include <stdexcept>
#include <cstdint>
#include <fstream>

namespace rt_tm {

	template<bool exceptions> class file_loader {
	  public:
		explicit file_loader(const std::filesystem::path& filePath) {
			if (!std::filesystem::exists(filePath)) {
				if constexpr (exceptions) {
					throw std::runtime_error("File does not exist: " + filePath.string());
				} else {
					std::cerr << "File does not exist: " + filePath.string() << std::endl;
				}
			}

			std::ifstream file(filePath, std::ios::binary | std::ios::ate);
			if (!file) {
				if constexpr (exceptions) {
					throw std::runtime_error("Failed to open file: " + filePath.string());
				} else {
					std::cerr << "Failed to open file: " + filePath.string() << std::endl;
				}
			}

			const std::streamsize size = file.tellg();
			file.seekg(0, std::ios::beg);

			contents.resize(static_cast<size_t>(size));
			if (!file.read(contents.data(), size)) {
				if constexpr (exceptions) {
					throw std::runtime_error("Failed to read file: " + filePath.string());
				} else {
					std::cerr << "Failed to read file: " + filePath.string() << std::endl;
				}
			}
		}

		operator const std::string&() const noexcept {
			return contents;
		}

		size_t size() const noexcept {
			return contents.size();
		}

	  private:
		std::string contents;
	};

	template<bool exceptions> class file_saver {
	  public:
		file_saver(const std::filesystem::path& path, const void* data, size_t size) {
			if (!data || size == 0) {
				if constexpr (exceptions) {
					throw std::runtime_error("Cannot save null or empty data to file: " + path.string());
				} else {
					std::cerr << "Cannot save null or empty data to file: " + path.string() << std::endl;
				}
			}

			std::ofstream file(path, std::ios::binary | std::ios::trunc);
			if (!file) {
				if constexpr (exceptions) {
					throw std::runtime_error("Failed to open file for writing: " + path.string());
				} else {
					std::cerr << "Failed to open file for writing: " + path.string() << std::endl;
				}
			}

			file.write(static_cast<const char*>(data), static_cast<std::streamsize>(size));
			if (!file) {
				if constexpr (exceptions) {
					throw std::runtime_error("Failed to write data to file: " + path.string());
				} else {
					std::cerr << "Failed to write data to file: " + path.string() << std::endl;
				}
			}
		}
	};

	template<typename value_type> struct debugging_io;

}
