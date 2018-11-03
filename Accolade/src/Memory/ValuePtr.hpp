#pragma once

#include <type_traits>

template<typename T>
class ValuePtr {
public:

	constexpr ValuePtr() noexcept {
		printf("Empty");
	}
	~ValuePtr() noexcept { if(ptr) delete ptr; }

	constexpr ValuePtr(T* ptr) noexcept : ptr(ptr) {
		printf("Private");
	}

	ValuePtr(const ValuePtr<T>& other) noexcept {
		printf("Me: %p, Other: %p\n", ptr, other.ptr);
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
		printf("Move");
		other.ptr = nullptr;
	};

	ValuePtr<T>& operator=(const ValuePtr<T>& other) noexcept {
		ptr = new T(*other.ptr);
		return *this;
	}

	constexpr ValuePtr<T>& operator=(ValuePtr<T>&& other) noexcept {
		ptr = other.ptr;
		other.ptr = nullptr;
		return *this;
	}

	constexpr operator bool() const noexcept {
		return ptr != nullptr;
	}

	constexpr T operator*() const noexcept {
		return *ptr;
	}
	constexpr T operator->() const noexcept {
		return *ptr;
	}

	constexpr T* get() const noexcept {
		return ptr;
	}
	[[nodiscard]]
	constexpr T* release() const noexcept {
		return ptr;
	}

private:
	template<typename U>
	friend class ValuePtr;

	T* ptr{ nullptr };
};