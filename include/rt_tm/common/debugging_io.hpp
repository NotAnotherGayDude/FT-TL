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
			if (size != -1) {
				contents.resize(static_cast<size_t>(size));
				if (!file.read(contents.data(), size)) {
					if constexpr (exceptions) {
						throw std::runtime_error("Failed to read file: " + filePath.string());
					} else {
						std::cerr << "Failed to read file: " + filePath.string() << std::endl;
					}
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

	static constexpr size_t FNV_OFFSET_BASIS_64{ 0xcbf29ce484222325ULL };
	static constexpr size_t FNV_PRIME_64{ 0x100000001b3ULL };

	RT_TM_FORCE_INLINE uint64_t fnv1a_hash64(const void* data, size_t size) {
		const uint8_t* bytes = static_cast<const uint8_t*>(data);
		uint64_t hash		 = FNV_OFFSET_BASIS_64;

		for (size_t i = 0; i < size; ++i) {
			hash ^= bytes[i];
			hash *= FNV_PRIME_64;
		}

		return hash;
	}

	struct intermediary_tensor {
		static constexpr size_t tensor_len_size{ sizeof(uint64_t) };
		static constexpr size_t name_len_size{ sizeof(uint64_t) };
		static constexpr size_t dims_size{ sizeof(size_t) * 4 };
		static constexpr size_t type_size{ sizeof(uint32_t) };
		std::vector<uint8_t> data{};
		array<size_t, 4> dims{};
		std::string name{};
		data_type type{};
		op_type op{};

		intermediary_tensor() noexcept = default;

		intermediary_tensor(const model_core& other) {
			size_t nbytes{ other.core_total_byte_size() };
			data.resize(nbytes);
			std::memcpy(data.data(), other.data.data(), nbytes);
			for (size_t x = 0; x < 4; ++x) {
				dims[x] = other.dims[x];
			}
			type = other.type;
			name = other.name;
		}

		RT_TM_FORCE_INLINE bool operator==(const intermediary_tensor& lhs) const {
			if (data != lhs.data) {
				for (size_t x = 0; x < lhs.data.size() && x < data.size(); ++x) {
					if (data[x] != lhs.data[x]) {
						//std::cout << "Lhs: " << data[x] << ", " << ", RHS: " << lhs.data[x] << std::endl;
					}
				}
			}
			return data == lhs.data && name == lhs.name && type == lhs.type && dims == lhs.dims;
		}

		/*
		intermediary_tensor(const ggml_tensor& other) {
			size_t nbytes{ ggml_nbytes(&other) };
			data.resize(nbytes);
			std::memcpy(data.data(), other.data, nbytes);
			for (size_t x = 0; x < 4; ++x) {
				dims[x] = other.ne[x];
			}
			type = static_cast<data_type>(other.type);
			name = std::string{other.name};
		}*/
	};

	RT_TM_FORCE_INLINE std::string serialize_tensor(const intermediary_tensor& other) {
		size_t string_size{ intermediary_tensor::type_size + intermediary_tensor::name_len_size + intermediary_tensor::tensor_len_size + intermediary_tensor::dims_size +
			other.data.size() + other.name.size() };
		std::string return_value{};
		return_value.resize(string_size);
		size_t current_offset{};
		std::memcpy(return_value.data(), &other.type, intermediary_tensor::type_size);
		current_offset += sizeof(other.type);
		std::memcpy(return_value.data() + current_offset, other.dims.data(), intermediary_tensor::dims_size);
		current_offset += intermediary_tensor::dims_size;
		uint64_t name_length{ other.name.size() };
		std::memcpy(return_value.data() + current_offset, &name_length, intermediary_tensor::name_len_size);
		current_offset += intermediary_tensor::name_len_size;
		std::memcpy(return_value.data() + current_offset, other.name.data(), name_length);
		current_offset += name_length;
		uint64_t tensor_length{ other.data.size() };
		std::memcpy(return_value.data() + current_offset, &tensor_length, intermediary_tensor::tensor_len_size);
		current_offset += intermediary_tensor::tensor_len_size;
		std::memcpy(return_value.data() + current_offset, other.data.data(), tensor_length);
		return return_value;
	}

	RT_TM_FORCE_INLINE intermediary_tensor parse_tensor(const std::string& other) {
		intermediary_tensor return_value{};
		size_t current_offset{};
		if (other.size() > 0) {
			std::memcpy(&return_value.type, other.data() + current_offset, intermediary_tensor::type_size);
			current_offset += intermediary_tensor::type_size;
			std::memcpy(return_value.dims.data(), other.data() + current_offset, intermediary_tensor::dims_size);
			current_offset += intermediary_tensor::dims_size;
			uint64_t name_length{};
			std::memcpy(&name_length, other.data() + current_offset, intermediary_tensor::name_len_size);
			current_offset += intermediary_tensor::name_len_size;
			return_value.name.resize(name_length);
			std::memcpy(return_value.name.data(), other.data() + current_offset, name_length);
			current_offset += name_length;
			uint64_t tensor_length{};
			std::memcpy(&tensor_length, other.data() + current_offset, intermediary_tensor::tensor_len_size);
			current_offset += intermediary_tensor::tensor_len_size;
			return_value.data.resize(tensor_length);
			std::memcpy(return_value.data.data(), other.data() + current_offset, tensor_length);
			current_offset += tensor_length;
		}
		return return_value;
	}

	template<bool exceptions, typename value_type> struct debugging_io;

	template<bool exceptions> struct debugging_io<exceptions, struct model_core> {
		RT_TM_FORCE_INLINE static void load_and_compare_tensors(const model_core& core) {
			auto new_string = file_loader<exceptions>{ core.name + ".safetensor" }.operator const std::string&();
			intermediary_tensor new_tensor{ parse_tensor(new_string) };
			intermediary_tensor save_tensor{ core };
			if (new_tensor != save_tensor) {
				std::cout << "Failed on Tensor: " << new_tensor.name << std::endl;
			} else {
				std::cout << "Success on Tensor: " << new_tensor.name << std::endl;
			}
		}
		RT_TM_FORCE_INLINE static void save_tensor(const model_core& core) {
			intermediary_tensor save_tensor{ core };
			std::string save_file{ serialize_tensor(save_tensor) };
			file_saver<exceptions>{ core.name + ".safetensor", save_file.data(), save_file.size() };
		}
	};
}
