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

#include <rt_tm/common/config.hpp>
#include <iterator>

namespace rt_tm {

	template<typename value_type_new, size_t size> class array_iterator {
	  public:
		using iterator_concept	= std::contiguous_iterator_tag;
		using iterator_category = std::random_access_iterator_tag;
		using element_type		= value_type_new;
		using value_type		= value_type_new;
		using difference_type	= std::ptrdiff_t;
		using pointer			= value_type*;
		using reference			= value_type&;

		RT_TM_FORCE_INLINE constexpr array_iterator() noexcept : ptr() {
		}

		RT_TM_FORCE_INLINE constexpr array_iterator(pointer ptrNew) noexcept : ptr(ptrNew) {
		}

		RT_TM_FORCE_INLINE constexpr reference operator*() const noexcept {
			return *ptr;
		}

		RT_TM_FORCE_INLINE constexpr pointer operator->() const noexcept {
			return std::pointer_traits<pointer>::pointer_to(**this);
		}

		RT_TM_FORCE_INLINE constexpr array_iterator& operator++() noexcept {
			++ptr;
			return *this;
		}

		RT_TM_FORCE_INLINE constexpr array_iterator operator++(int32_t) noexcept {
			array_iterator temp = *this;
			++*this;
			return temp;
		}

		RT_TM_FORCE_INLINE constexpr array_iterator& operator--() noexcept {
			--ptr;
			return *this;
		}

		RT_TM_FORCE_INLINE constexpr array_iterator operator--(int32_t) noexcept {
			array_iterator temp = *this;
			--*this;
			return temp;
		}

		RT_TM_FORCE_INLINE constexpr array_iterator& operator+=(const difference_type offSet) noexcept {
			ptr += offSet;
			return *this;
		}

		RT_TM_FORCE_INLINE constexpr array_iterator operator+(const difference_type offSet) const noexcept {
			array_iterator temp = *this;
			temp += offSet;
			return temp;
		}

		RT_TM_FORCE_INLINE friend constexpr array_iterator operator+(const difference_type offSet, array_iterator _Next) noexcept {
			_Next += offSet;
			return _Next;
		}

		RT_TM_FORCE_INLINE constexpr array_iterator& operator-=(const difference_type offSet) noexcept {
			return *this += -offSet;
		}

		RT_TM_FORCE_INLINE constexpr array_iterator operator-(const difference_type offSet) const noexcept {
			array_iterator temp = *this;
			temp -= offSet;
			return temp;
		}

		RT_TM_FORCE_INLINE constexpr difference_type operator-(const array_iterator& other) const noexcept {
			return static_cast<difference_type>(ptr - other.ptr);
		}

		RT_TM_FORCE_INLINE constexpr reference operator[](const difference_type offSet) const noexcept {
			return *(*this + offSet);
		}

		RT_TM_FORCE_INLINE constexpr bool operator==(const array_iterator& other) const noexcept {
			return ptr == other.ptr;
		}

		RT_TM_FORCE_INLINE constexpr std::strong_ordering operator<=>(const array_iterator& other) const noexcept {
			return ptr <=> other.ptr;
		}

		RT_TM_FORCE_INLINE constexpr bool operator!=(const array_iterator& other) const noexcept {
			return !(*this == other);
		}

		RT_TM_FORCE_INLINE constexpr bool operator<(const array_iterator& other) const noexcept {
			return ptr < other.ptr;
		}

		RT_TM_FORCE_INLINE constexpr bool operator>(const array_iterator& other) const noexcept {
			return other < *this;
		}

		RT_TM_FORCE_INLINE constexpr bool operator<=(const array_iterator& other) const noexcept {
			return !(other < *this);
		}

		RT_TM_FORCE_INLINE constexpr bool operator>=(const array_iterator& other) const noexcept {
			return !(*this < other);
		}

		pointer ptr;
	};

	template<typename value_type_new> class array_iterator<value_type_new, 0> {
	  public:
		using iterator_concept	= std::contiguous_iterator_tag;
		using iterator_category = std::random_access_iterator_tag;
		using element_type		= value_type_new;
		using value_type		= value_type_new;
		using difference_type	= std::ptrdiff_t;
		using pointer			= value_type*;
		using reference			= value_type&;

		RT_TM_FORCE_INLINE constexpr array_iterator() noexcept {
		}

		RT_TM_FORCE_INLINE constexpr array_iterator(std::nullptr_t ptrNew) noexcept {
			( void )ptrNew;
		}

		RT_TM_FORCE_INLINE constexpr bool operator==(const array_iterator& other) const noexcept {
			( void )other;
			return true;
		}

		RT_TM_FORCE_INLINE constexpr bool operator!=(const array_iterator& other) const noexcept {
			return !(*this == other);
		}

		RT_TM_FORCE_INLINE constexpr bool operator>=(const array_iterator& other) const noexcept {
			return !(*this < other);
		}
	};

}
