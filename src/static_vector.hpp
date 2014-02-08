#pragma once

namespace util {
	template <typename T, size_t N>
	struct static_vector {
		typedef T value_type;
		typedef T* iterator;
		typedef const T* const_iterator;
		typedef T&                                    reference;
		typedef const T&                              const_reference;
		typedef std::size_t                           size_type;
		typedef std::ptrdiff_t                        difference_type;

		static const size_type static_size = N;

		iterator begin() { return _elems; }
		const_iterator begin() const { return _elems; }
		const_iterator cbegin() const { return _elems; }
		iterator end() { return _elems + _count; }
		const_iterator end() const { return _elems + _count; }
		const_iterator cend() const { return _elems + _count; }

		void clear() {
			_count = 0;
		}

		void resize(size_type sz) {
			_count = sz;
		}

		void resize_max() {
			_count = N;
		}

		size_type size() const {
			return _count;
		}

		bool empty() const {
			return _count == 0;
		}

		size_type max_size() {
			return N;
		}

		reference operator[](size_type i) {
			return _elems[i];
		}

		const_reference operator[](size_type i) const {
			return _elems[i];
		}

		reference front() {
			return _elems[0];
		}

		const_reference front() const {
			return _elems[0];
		}

		reference back() {
			return _elems[_count-1];
		}

		const_reference back() const {
			return _elems[_count-1];
		}

		const T* data() const { return _elems; }
		T* data() { return _elems; }

		T _elems[N];
		size_type _count;
	};
}
