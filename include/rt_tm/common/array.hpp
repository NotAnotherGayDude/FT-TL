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

#include <rt_tm/common/iterator.hpp>
#include <algorithm>

namespace rt_tm {

	template<typename value_type_new, size_t size_new> struct array {
	  public:
		static constexpr size_t size_val{ size_new };
		using value_type			 = value_type_new;
		using size_type				 = size_t;
		using difference_type		 = ptrdiff_t;
		using pointer				 = value_type*;
		using const_pointer			 = const value_type*;
		using reference				 = value_type&;
		using const_reference		 = const value_type&;
		using iterator				 = array_iterator<value_type, size_new>;
		using const_iterator		 = const array_iterator<value_type, size_new>;
		using reverse_iterator		 = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		RT_TM_FORCE_INLINE constexpr array() noexcept = default;

		RT_TM_FORCE_INLINE constexpr array(const std::initializer_list<value_type>& values) {
			std::copy_n(values.begin(), values.size(), data_val);
		}

		RT_TM_FORCE_INLINE constexpr array(const value_type (&values)[size_new]) {
			std::copy_n(&values, size_new, data_val);
		}

		RT_TM_FORCE_INLINE constexpr void fill(const value_type& _Value) {
			std::fill_n(data_val, size_new, _Value);
		}

		RT_TM_FORCE_INLINE constexpr iterator begin() noexcept {
			return iterator(data_val);
		}

		RT_TM_FORCE_INLINE constexpr const_iterator begin() const noexcept {
			return const_iterator(data_val);
		}

		RT_TM_FORCE_INLINE constexpr iterator end() noexcept {
			return iterator(data_val);
		}

		RT_TM_FORCE_INLINE constexpr const_iterator end() const noexcept {
			return const_iterator(data_val);
		}

		RT_TM_FORCE_INLINE constexpr reverse_iterator rbegin() noexcept {
			return reverse_iterator(end());
		}

		RT_TM_FORCE_INLINE constexpr const_reverse_iterator rbegin() const noexcept {
			return const_reverse_iterator(end());
		}

		RT_TM_FORCE_INLINE constexpr reverse_iterator rend() noexcept {
			return reverse_iterator(begin());
		}

		RT_TM_FORCE_INLINE constexpr const_reverse_iterator rend() const noexcept {
			return const_reverse_iterator(begin());
		}

		RT_TM_FORCE_INLINE constexpr const_iterator cbegin() const noexcept {
			return begin();
		}

		RT_TM_FORCE_INLINE constexpr const_iterator cend() const noexcept {
			return end();
		}

		RT_TM_FORCE_INLINE constexpr const_reverse_iterator crbegin() const noexcept {
			return rbegin();
		}

		RT_TM_FORCE_INLINE constexpr const_reverse_iterator crend() const noexcept {
			return rend();
		}

		RT_TM_FORCE_INLINE constexpr size_type size() const noexcept {
			return size_new;
		}

		RT_TM_FORCE_INLINE constexpr size_type max_size() const noexcept {
			return size_new;
		}

		RT_TM_FORCE_INLINE constexpr bool empty() const noexcept {
			return false;
		}

		RT_TM_FORCE_INLINE constexpr reference at(size_type position) {
			if (size_new <= position) {
				std::runtime_error{ "invalid array<T, N> subscript" };
			}

			return data_val[position];
		}

		RT_TM_FORCE_INLINE constexpr const_reference at(size_type position) const {
			if (size_new <= position) {
				std::runtime_error{ "invalid array<T, N> subscript" };
			}

			return data_val[position];
		}

		RT_TM_FORCE_INLINE constexpr reference operator[](size_type position) noexcept {
			return data_val[position];
		}

		RT_TM_FORCE_INLINE constexpr const_reference operator[](size_type position) const noexcept {
			return data_val[position];
		}

		RT_TM_FORCE_INLINE constexpr reference front() noexcept {
			return data_val[0];
		}

		RT_TM_FORCE_INLINE constexpr const_reference front() const noexcept {
			return data_val[0];
		}

		RT_TM_FORCE_INLINE constexpr reference back() noexcept {
			return data_val[size_new - 1];
		}

		RT_TM_FORCE_INLINE constexpr const_reference back() const noexcept {
			return data_val[size_new - 1];
		}

		RT_TM_FORCE_INLINE constexpr value_type* data() noexcept {
			return data_val;
		}

		RT_TM_FORCE_INLINE constexpr const value_type* data() const noexcept {
			return data_val;
		}

		RT_TM_FORCE_INLINE constexpr friend bool operator==(const array& lhs, const array& rhs) {
			if (lhs.size_val == rhs.size_val) {
				for (size_t x = 0; x < size_val; ++x) {
					if (lhs[x] != rhs[x]) {
						return false;
					}
				}
				return true;
			} else {
				return false;
			}
		}

		value_type data_val[size_val]{};
	};

	struct empty_array_element {};

	template<class value_type_new> class array<value_type_new, 0> {
	  public:
		using value_type			 = value_type_new;
		using size_type				 = size_t;
		using difference_type		 = ptrdiff_t;
		using pointer				 = value_type*;
		using const_pointer			 = const value_type*;
		using reference				 = value_type&;
		using const_reference		 = const value_type&;
		using iterator				 = array_iterator<value_type, 0>;
		using const_iterator		 = const array_iterator<value_type, 0>;
		using reverse_iterator		 = std::reverse_iterator<iterator>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;

		RT_TM_FORCE_INLINE constexpr void fill(const value_type&) {
		}

		RT_TM_FORCE_INLINE constexpr void swap(array&) noexcept {
		}

		RT_TM_FORCE_INLINE constexpr iterator begin() noexcept {
			return iterator{};
		}

		RT_TM_FORCE_INLINE constexpr const_iterator begin() const noexcept {
			return const_iterator{};
		}

		RT_TM_FORCE_INLINE constexpr iterator end() noexcept {
			return iterator{};
		}

		RT_TM_FORCE_INLINE constexpr const_iterator end() const noexcept {
			return const_iterator{};
		}

		RT_TM_FORCE_INLINE constexpr reverse_iterator rbegin() noexcept {
			return reverse_iterator(end());
		}

		RT_TM_FORCE_INLINE constexpr const_reverse_iterator rbegin() const noexcept {
			return const_reverse_iterator(end());
		}

		RT_TM_FORCE_INLINE constexpr reverse_iterator rend() noexcept {
			return reverse_iterator(begin());
		}

		RT_TM_FORCE_INLINE constexpr const_reverse_iterator rend() const noexcept {
			return const_reverse_iterator(begin());
		}

		RT_TM_FORCE_INLINE constexpr const_iterator cbegin() const noexcept {
			return begin();
		}

		RT_TM_FORCE_INLINE constexpr const_iterator cend() const noexcept {
			return end();
		}

		RT_TM_FORCE_INLINE constexpr const_reverse_iterator crbegin() const noexcept {
			return rbegin();
		}

		RT_TM_FORCE_INLINE constexpr const_reverse_iterator crend() const noexcept {
			return rend();
		}

		RT_TM_FORCE_INLINE constexpr size_type size() const noexcept {
			return 0;
		}

		RT_TM_FORCE_INLINE constexpr size_type max_size() const noexcept {
			return 0;
		}

		RT_TM_FORCE_INLINE constexpr bool empty() const noexcept {
			return true;
		}

		RT_TM_FORCE_INLINE constexpr reference at(size_type) {
			std::runtime_error{ "invalid array<T, N> subscript" };
		}

		RT_TM_FORCE_INLINE constexpr const_reference at(size_type) const {
			std::runtime_error{ "invalid array<T, N> subscript" };
		}

		RT_TM_FORCE_INLINE constexpr reference operator[](size_type) noexcept {
			return *data();
		}

		RT_TM_FORCE_INLINE constexpr const_reference operator[](size_type) const noexcept {
			return *data();
		}

		RT_TM_FORCE_INLINE constexpr reference front() noexcept {
			return *data();
		}

		RT_TM_FORCE_INLINE constexpr const_reference front() const noexcept {
			return *data();
		}

		RT_TM_FORCE_INLINE constexpr reference back() noexcept {
			return *data();
		}

		RT_TM_FORCE_INLINE constexpr const_reference back() const noexcept {
			return *data();
		}

		RT_TM_FORCE_INLINE constexpr value_type* data() noexcept {
			return nullptr;
		}

		RT_TM_FORCE_INLINE constexpr const value_type* data() const noexcept {
			return nullptr;
		}

		RT_TM_FORCE_INLINE constexpr friend bool operator==(const array& lhs, const array& rhs) {
			( void )lhs;
			( void )rhs;
			return true;
		}

	  private:
		std::conditional_t<std::disjunction_v<std::is_default_constructible<value_type>, std::is_default_constructible<value_type>>, value_type, empty_array_element> data_val[1]{};
	};
}
