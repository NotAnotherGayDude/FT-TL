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

#include <nihilus/cpu/simd/nihilus_cpu_instructions.hpp>
#include <nihilus/common/string_literal.hpp>
#include <nihilus/common/data_types.hpp>
#include <nihilus/common/concepts.hpp>
#include <iostream>
#include <cstdint>
#include <chrono>
#include <thread>
#include <mutex>
#include <latch>
#include <cmath>

namespace nihilus {

	struct alignas(64) atomic_flag_wrapper {
		NIHILUS_FORCE_INLINE atomic_flag_wrapper() noexcept = default;
		NIHILUS_FORCE_INLINE atomic_flag_wrapper& operator=(const atomic_flag_wrapper&) noexcept {
			return *this;
		};
		NIHILUS_FORCE_INLINE atomic_flag_wrapper(const atomic_flag_wrapper&) noexcept {};

		alignas(64) std::atomic_flag flag{};

		NIHILUS_FORCE_INLINE void clear(std::memory_order order) {
			flag.clear(order);
		}

		NIHILUS_FORCE_INLINE void test_and_set(std::memory_order order) {
			flag.test_and_set(order);
		}

		NIHILUS_FORCE_INLINE void notify_one() {
			flag.notify_one();
		}

		NIHILUS_FORCE_INLINE bool test(std::memory_order order) {
			return flag.test(order);
		}

		NIHILUS_FORCE_INLINE void wait(bool value) {
			flag.wait(value);
		}
	};

	struct alignas(64) slim_latch {
		alignas(64) std::vector<atomic_flag_wrapper> thread_flags{};
		alignas(64) std::atomic<uint64_t> global_counter{};
		alignas(64) size_t thread_count{};

		NIHILUS_FORCE_INLINE slim_latch() noexcept = default;
		NIHILUS_FORCE_INLINE slim_latch(const slim_latch&) noexcept {};

		NIHILUS_FORCE_INLINE void reset(uint64_t count) {
			thread_count = count;
			global_counter.store(count, std::memory_order_release);
			thread_flags.resize(count);
			for (size_t i = 0; i < count; ++i) {
				thread_flags[i].clear(std::memory_order_release);
			}
		}

		NIHILUS_FORCE_INLINE bool try_wait() {
			return global_counter.load(std::memory_order_acquire) == 0;
		}

		NIHILUS_FORCE_INLINE void count_down() {
			global_counter.store(0, std::memory_order_release);
			global_counter.notify_all();

			for (size_t x = 0; x < thread_count; ++x) {
				thread_flags[x].test_and_set(std::memory_order_release);
				thread_flags[x].notify_one();
			}
		}

		NIHILUS_FORCE_INLINE void count_up() {
			global_counter.store(thread_count, std::memory_order_release);
			global_counter.notify_all();

			for (size_t x = 0; x < thread_count; ++x) {
				thread_flags[x].test_and_set(std::memory_order_release);
				thread_flags[x].notify_one();
			}
		}

		NIHILUS_FORCE_INLINE void arrive_and_wait(size_t thread_index) {
			uint64_t remaining = global_counter.fetch_sub(1, std::memory_order_acq_rel);

			if (remaining == 1) {
				count_down();
			} else {
				bool current_value = thread_flags[thread_index].test(std::memory_order_acquire);
				while (!current_value) {
					for (int i = 0; i < 100 && !current_value; ++i) {
						current_value = thread_flags[thread_index].test(std::memory_order_acquire);
					}
					nihilus_pause();
				}
			}
		}

		NIHILUS_FORCE_INLINE void arrive_and_wait_second(size_t thread_index) {
			uint64_t remaining = global_counter.fetch_add(1, std::memory_order_acq_rel);

			if (remaining == thread_count - 1) {
				count_up();
			} else {
				bool current_value = thread_flags[thread_index].test(std::memory_order_acquire);
				while (!current_value) {
					for (int i = 0; i < 100 && !current_value; ++i) {
						current_value = thread_flags[thread_index].test(std::memory_order_acquire);
					}
					nihilus_pause();
				}
			}
		}

		NIHILUS_FORCE_INLINE void wait() {
			uint64_t current_value = global_counter.load(std::memory_order_acquire);
			while (current_value > 0) {
				for (int i = 0; i < 100 && current_value > 0; ++i) {
					current_value = global_counter.load(std::memory_order_acquire);
				}
				nihilus_pause();
			}
		}

		NIHILUS_FORCE_INLINE void wait_second() {
			uint64_t current_value = global_counter.load(std::memory_order_acquire);
			while (current_value < thread_count) {
				for (int i = 0; i < 100 && current_value > 0; ++i) {
					current_value = global_counter.load(std::memory_order_acquire);
				}
				nihilus_pause();
			}
		}
	};

	template<typename value_type>
	concept time_t = is_specialization_v<value_type, std::chrono::duration>;

	template<time_t value_type = std::chrono::nanoseconds> class stop_watch {
	  public:
		using hr_clock = std::conditional_t<std::chrono::high_resolution_clock::is_steady, std::chrono::high_resolution_clock, std::chrono::steady_clock>;
		static constexpr bool lock_free{ std::atomic<value_type>::is_always_lock_free };
		using time_type = std::conditional_t<lock_free, value_type, uint64_t>;

		NIHILUS_FORCE_INLINE stop_watch(uint64_t newTime) noexcept {
			total_time_units.store(time_type{ newTime }, std::memory_order_release);
		}

		NIHILUS_FORCE_INLINE stop_watch& operator=(stop_watch&& other) noexcept {
			if NIHILUS_LIKELY (this != &other) {
				total_time_units.store(other.total_time_units.load(std::memory_order_acquire), std::memory_order_release);
				start_time_units.store(other.start_time_units.load(std::memory_order_acquire), std::memory_order_release);
			}
			return *this;
		}

		NIHILUS_FORCE_INLINE stop_watch(stop_watch&& other) noexcept {
			*this = std::move(other);
		}

		NIHILUS_FORCE_INLINE stop_watch& operator=(const stop_watch& other) noexcept {
			if NIHILUS_LIKELY (this != &other) {
				total_time_units.store(other.total_time_units.load(std::memory_order_acquire), std::memory_order_release);
				start_time_units.store(other.start_time_units.load(std::memory_order_acquire), std::memory_order_release);
			}
			return *this;
		}

		NIHILUS_FORCE_INLINE stop_watch(const stop_watch& other) noexcept {
			*this = other;
		}

		NIHILUS_FORCE_INLINE bool has_time_elapsed() noexcept {
			return ((get_current_time() - start_time_units.load(std::memory_order_acquire)) >= total_time_units.load(std::memory_order_acquire));
		}

		NIHILUS_FORCE_INLINE void add_time() noexcept {
			//std::unique_lock lock{ mutex };
			values.emplace_back(total_time_elapsed());
			//lock.release();
			reset();
		}

		NIHILUS_FORCE_INLINE uint64_t get_average(time_type newTimeValue = time_type{}) noexcept {
			std::unique_lock lock{ mutex };
			uint64_t total_time{};
			for (auto& value: values) {
				total_time += get_value_as_uint(value);
			}
			return total_time / ((values.size() > 0) ? values.size() : 1);
		}

		NIHILUS_FORCE_INLINE uint64_t get_count() noexcept {
			return values.size();
		}

		NIHILUS_FORCE_INLINE void reset(time_type newTimeValue = time_type{}) noexcept {
			if NIHILUS_LIKELY (newTimeValue != time_type{}) {
				total_time_units.store(newTimeValue, std::memory_order_release);
			}
			start_time_units.store(get_current_time(), std::memory_order_release);
		}

		NIHILUS_FORCE_INLINE uint64_t get_total_wait_time() const noexcept {
			return get_value_as_uint(total_time_units.load(std::memory_order_acquire));
		}

		NIHILUS_FORCE_INLINE time_type total_time_elapsed() noexcept {
			return get_current_time() - start_time_units.load(std::memory_order_acquire);
		}

		NIHILUS_FORCE_INLINE uint64_t total_time_elapsed_uint64() noexcept {
			return get_value_as_uint(get_current_time()) - get_value_as_uint(start_time_units.load(std::memory_order_acquire));
		}

	  protected:
		std::atomic<time_type> total_time_units{};
		std::atomic<time_type> start_time_units{};
		std::vector<time_type> values{};
		std::mutex mutex{};

		NIHILUS_FORCE_INLINE time_type get_current_time() {
			if constexpr (lock_free) {
				return std::chrono::duration_cast<value_type>(hr_clock::now().time_since_epoch());
			} else {
				return std::chrono::duration_cast<value_type>(hr_clock::now().time_since_epoch()).count();
			}
		}

		NIHILUS_FORCE_INLINE uint64_t get_value_as_uint(time_type time) {
			if constexpr (lock_free) {
				return time.count();
			} else {
				return time;
			}
		}
	};

	inline std::mutex mutex{};

	NIHILUS_FORCE_INLINE void log(std::string_view string) {
		std::unique_lock lock{ mutex };
		std::cout << string << std::endl;
	}

	template<auto current_index, auto enum_count> NIHILUS_FORCE_INLINE constexpr std::string_view get_enum_name() {
		std::string_view return_string{ std::source_location::current().function_name() };
		auto new_size	   = std::size("get_enum_name<");
		uint64_t new_index = return_string.find("get_enum_name<") + new_size - 1;
		return_string	   = return_string.substr(new_index, return_string.size() - new_index);
		return_string	   = return_string.substr(0, return_string.find(','));
		return return_string;
	}

	template<auto current_index, auto enum_count> NIHILUS_FORCE_INLINE std::string print_enum_value(auto enum_val) {
		if constexpr (static_cast<uint64_t>(current_index) < static_cast<uint64_t>(enum_count)) {
			if (static_cast<uint64_t>(current_index) == static_cast<uint64_t>(enum_val)) {
				constexpr std::string_view string{ get_enum_name<current_index, enum_count>() };
				return static_cast<std::string>(string);
			} else {
				return print_enum_value<static_cast<decltype(enum_count)>(static_cast<uint64_t>(current_index) + 1), enum_count>(enum_val);
			}
		} else {
			return {};
		}
	};

	enum class data_type : uint64_t {
		f32	 = 0,
		f16	 = 1,
		q8_0 = 8,
		i8	 = 24,
		i16	 = 25,
		i32	 = 26,
		i64	 = 27,
		f64	 = 28,
		count,
	};

	NIHILUS_FORCE_INLINE constexpr const char* get_type_name(data_type type) {
		switch (type) {
			case data_type::f32: {
				return "float_32";
			}
			case data_type::f16: {
				return "float_16";
			}
			case data_type::q8_0: {
				return "q8_0";
			}
			case data_type::i8: {
				return "int8_t";
			}
			case data_type::i16: {
				return "int16_t";
			}
			case data_type::i32: {
				return "int32_t";
			}
			case data_type::i64: {
				return "int64_t";
			}
		}
	}

	enum class kernel_type : uint8_t {
		none,
		get_rows,
		rms_norm,
		mul,
		mul_mat,
		reshape,
		permute,
		transpose,
		view,
		cont,
		copy,
		rope,
		softmax,
		silu,
		add,
		sub,
		count,
	};

	static constexpr array<const char*, kernel_type::count> kernel_names{ { "none", "get_rows", "rms_norm", "mul", "mul_mat", "reshape", "permute", "transpose", "view", "cont",
		"copy", "rope", "softmax", "silu", "add", "sub" } };

	enum class llama_op_types : uint16_t {
		inp_embd,
		token_embd_weight,
		inp_tokens,
		inp_pos,
		inp_out_ids,
		rope_freqs_weight,
		output_weight,
		output_norm_weight,
		attn_q_weight,
		attn_k_weight,
		attn_v_weight,
		attn_output_weight,
		attn_norm_weight,
		ffn_gate_weight,
		ffn_up_weight,
		ffn_down_weight,
		ffn_norm_weight,
		cache_k,
		cache_v,
		kq_mask,
		norm,
		attn_norm,
		qcur,
		qcur_reshaped,
		qcur_rope,
		kcur,
		kcur_reshaped,
		kcur_rope,
		vcur,
		k_cache_view,
		k_cache_view_copy,
		vcur_transposed,
		v_cache_view,
		v_cache_view_copy,
		v,
		k,
		q,
		kq,
		kq_soft_max,
		kqv,
		kqv_merged,
		kqv_merged_cont,
		kqv_out,
		ffn_inp,
		norm_out,
		ffn_norm,
		ffn_gate,
		ffn_silu,
		ffn_up,
		ffn_gate_par,
		ffn_out,
		l_out,
		attn_residual,
		prev_residual,
		final_norm,
		result_norm,
		result_output,
		count
	};

	static constexpr array<const char*, llama_op_types::count> llama_op_names{ { "inp_embd", "token_embd_weight", "inp_tokens", "inp_pos", "inp_out_ids", "rope_freqs_weight",
		"output_weight", "output_norm_weight", "attn_q_weight", "attn_k_weight", "attn_v_weight", "attn_output_weight", "attn_norm_weight", "ffn_gate_weight", "ffn_up_weight",
		"ffn_down_weight", "ffn_norm_weight", "cache_k", "cache_v", "kq_mask", "norm", "attn_norm", "qcur", "qcur_reshaped", "qcur_rope", "kcur", "kcur_reshaped", "kcur_rope",
		"vcur", "k_cache_view", "k_cache_view_copy", "vcur_transposed", "v_cache_view", "v_cache_view_copy", "v", "k", "q", "kq", "kq_soft_max", "kqv", "kqv_merged",
		"kqv_merged_cont", "kqv_out", "ffn_inp", "norm_out", "ffn_norm", "ffn_gate", "ffn_silu", "ffn_up", "ffn_gate_par", "ffn_out", "l_out", "attn_residual", "prev_residual",
		"final_norm", "result_norm", "result_output" } };

	template<integral_or_enum value_type> constexpr kernel_type get_kernel_type_from_llama_op(value_type op) {
		switch (static_cast<llama_op_types>(op)) {
			case llama_op_types::inp_tokens:
			case llama_op_types::inp_pos:
			case llama_op_types::inp_out_ids:
			case llama_op_types::token_embd_weight:
			case llama_op_types::rope_freqs_weight:
			case llama_op_types::output_weight:
			case llama_op_types::output_norm_weight:
			case llama_op_types::attn_q_weight:
			case llama_op_types::attn_k_weight:
			case llama_op_types::attn_v_weight:
			case llama_op_types::attn_output_weight:
			case llama_op_types::attn_norm_weight:
			case llama_op_types::ffn_gate_weight:
			case llama_op_types::ffn_up_weight:
			case llama_op_types::ffn_down_weight:
			case llama_op_types::ffn_norm_weight:
			case llama_op_types::cache_k:
			case llama_op_types::cache_v:
			case llama_op_types::kq_mask:
				return kernel_type::none;
			case llama_op_types::inp_embd:
			case llama_op_types::attn_residual:
			case llama_op_types::prev_residual:
				return kernel_type::get_rows;
			case llama_op_types::norm:
			case llama_op_types::norm_out:
			case llama_op_types::ffn_norm:
			case llama_op_types::final_norm:
				return kernel_type::rms_norm;
			case llama_op_types::attn_norm:
			case llama_op_types::ffn_gate_par:
			case llama_op_types::result_norm:
				return kernel_type::mul;
			case llama_op_types::qcur:
			case llama_op_types::kcur:
			case llama_op_types::vcur:
			case llama_op_types::kq:
			case llama_op_types::kqv:
			case llama_op_types::kqv_out:
			case llama_op_types::ffn_gate:
			case llama_op_types::ffn_up:
			case llama_op_types::ffn_out:
			case llama_op_types::result_output:
				return kernel_type::mul_mat;
			case llama_op_types::qcur_reshaped:
			case llama_op_types::kcur_reshaped:
				return kernel_type::reshape;
			case llama_op_types::q:
			case llama_op_types::kqv_merged:
				return kernel_type::permute;
			case llama_op_types::vcur_transposed:
				return kernel_type::transpose;
			case llama_op_types::k_cache_view:
			case llama_op_types::v_cache_view:
			case llama_op_types::v:
			case llama_op_types::k:
				return kernel_type::view;
			case llama_op_types::kqv_merged_cont:
				return kernel_type::cont;
			case llama_op_types::k_cache_view_copy:
			case llama_op_types::v_cache_view_copy:
				return kernel_type::copy;
			case llama_op_types::qcur_rope:
			case llama_op_types::kcur_rope:
				return kernel_type::rope;
			case llama_op_types::kq_soft_max:
				return kernel_type::softmax;
			case llama_op_types::ffn_silu:
				return kernel_type::silu;
			case llama_op_types::ffn_inp:
			case llama_op_types::l_out:
				return kernel_type::add;
			case llama_op_types::count:
			default:
				return kernel_type::none;
		}
	}

	enum class device_type {
		cpu,
		gpu,
		numa,
	};

	enum class model_arch {
		llama,
		count,
	};

	enum class kernel_type_profile : uint64_t {
		fp16_mha,
		fp16_moe,
		bf16_mha,
		bf16_gqa,
		q4_mha,
		q4_gqa,
		q4_moe,
		q8_mha,
		q8_gqa,
		q8_moe,
		mixed_fp16_fp32,
		mixed_bf16_fp32,
		count,
	};

	enum class norm_type : uint64_t {
		rms_standard,
		rms_parallel,
		rms_grouped,
		layer_norm_standard,
		layer_norm_no_bias,
		rms_norm_welford,
		adaptive_norm,
		count,
	};

	enum class kv_cache_strategy : uint64_t {
		contiguous,
		paged,
		compressed,
		streaming,
		hierarchical,
		count,
	};

	enum class rope_scaling_type : uint64_t {
		none,
		linear,
		dynamic,
		yarn,
		longrope,
		count,
	};

	enum class llama_model_generation : uint64_t {
		v1_v2,
		v3,
		count,
	};

	enum class llama_model_size {
		llama_1B,
		llama_3B,
		llama_7B,
		llama_8B,
		llama_11B,
		llama_13B,
		llama_70B,
		llama_90B,
		llama_405B,
		count,
	};

	template<typename model_size_type> struct get_op_type_type {
		static constexpr auto get_op_type_impl() {
			if constexpr (std::is_same_v<llama_model_generation, std::remove_cvref_t<model_size_type>>) {
				return llama_op_types{};
			} else {
				return size_t{};
			}
		}

		using type = decltype(get_op_type_impl());
	};

	enum class model_format { gguf = 1 };

	template<typename model_generation_type_new, typename model_size_type_new> struct model_config {
		using model_generation_type = model_generation_type_new;
		using model_size_type		= model_size_type_new;
		using op_type_type			= typename get_op_type_type<model_size_type>::type;
		model_generation_type model_generation{};
		model_size_type model_size{};
		kernel_type_profile kernel_profile{};
		model_arch arch{};
		kv_cache_strategy cache_strategy{};
		bool use_gradient_checkpointing{};
		rope_scaling_type rope_scaling{};
		bool use_rotary_embeddings{};
		uint64_t kv_cache_block_size{};
		bool use_flash_attention{};
		norm_type rms_norm_type{};
		model_format format{};
		float norm_epsilon{};
		bool exceptions{};

	  protected:
		template<typename model_generateion_type_newer, typename model_size_type_newer> friend struct model_base;
		friend struct harbinger;

		constexpr model_config(auto model_generation_new, auto model_size_new, kernel_type_profile kernel_profile_new, model_arch arch_new, bool exceptions_new,
			kv_cache_strategy cache_strategy_new, bool use_gradient_checkpointing_new, rope_scaling_type rope_scaling_new, bool use_rotary_embeddings_new,
			uint64_t kv_cache_block_size_new, bool use_flash_attention_new, norm_type rms_norm_type_new, model_format format_new, float norm_epsilon_new)
			: model_generation(model_generation_new), model_size(model_size_new), kernel_profile(kernel_profile_new), arch(arch_new), cache_strategy(cache_strategy_new),
			  use_gradient_checkpointing(use_gradient_checkpointing_new), rope_scaling(rope_scaling_new), use_rotary_embeddings(use_rotary_embeddings_new),
			  kv_cache_block_size(kv_cache_block_size_new), use_flash_attention(use_flash_attention_new), rms_norm_type(rms_norm_type_new), format{ format_new },
			  norm_epsilon(norm_epsilon_new), exceptions(exceptions_new) {};

		constexpr model_config() = default;
	};

	template<model_config config> using get_op_type_type_t = get_op_type_type<typename decltype(config)::model_size_type>::type;

	struct cli_params {
		uint64_t thread_count{ std::thread::hardware_concurrency() };
		bool no_conversation{ false };
		uint64_t batch_size{ 512 };
		uint64_t n_predict{ 128 };
		std::string model_file{};
		uint64_t n_tokens{ 0 };
		std::string prompt{};
		uint64_t seed{ 0 };
	};

	struct impl_indices {
		uint64_t cpu_index{};
		uint64_t gpu_index{};
	};

	struct op_graph_config {
		uint64_t num_threads{ std::thread::hardware_concurrency() };
	};

	struct execution_parameters {
		const int32_t* input_tokens{};
		size_t kv_cache_seq_len{};
		size_t position_offset{};
		size_t max_new_tokens{};
		uint64_t random_seed{};
		int32_t eos_token_id{};
		bool clear_kv_cache{};
		size_t token_count{};
		size_t sequence_id{};
		size_t batch_size{};
		float temperature{};
		bool is_prefill{};
		bool use_cache{};
		int32_t top_k{};
		float top_p{};
	};
}
