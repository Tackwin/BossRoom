#pragma once

#include <type_traits>
#include "Utils/meta_algorithms.hpp"

template<typename T>
class ValuePtr {
public:

	constexpr ValuePtr() noexcept {}
	~ValuePtr() noexcept { if(ptr) delete ptr; }

	constexpr ValuePtr(T* ptr) noexcept : ptr(ptr) {}

	ValuePtr(const ValuePtr<T>& other) noexcept {
		if constexpr (has_clone_v<T>) {
			ptr = other.ptr->clone();
		}
		else {
			ptr = new T(*other.ptr);
		}
	}

	template<
		typename U,
		typename = std::enable_if_t<std::is_base_of_v<T, U>, int>
	>
	constexpr ValuePtr(ValuePtr<U>&& other) noexcept : ptr((T*)other.ptr) {
		other.ptr = nullptr;
	};

	ValuePtr<T>& operator=(const ValuePtr<T>& other) noexcept {
		if (ptr) delete ptr;
		if constexpr (has_clone_v<T>) {
			ptr = other.ptr->clone();
		}
		else {
			ptr = new T(*other.ptr);
		}
		return *this;
	}

	constexpr ValuePtr<T>& operator=(ValuePtr<T>&& other) noexcept {
		if (ptr) delete ptr;
		ptr = other.ptr;
		other.ptr = nullptr;
		return *this;
	}

	constexpr operator bool() const noexcept {
		return ptr != nullptr;
	}

	constexpr T& operator*() const noexcept {
		return *ptr;
	}
	constexpr T& operator->() const noexcept {
		return *ptr;
	}

	constexpr T* get() const noexcept {
		return ptr;
	}
	[[nodiscard]]
	constexpr T* release() noexcept {
		auto temp = ptr;
		ptr = nullptr;
		return temp;
	}

private:
	template<typename U>
	friend class ValuePtr;

	T* ptr{ nullptr };
};