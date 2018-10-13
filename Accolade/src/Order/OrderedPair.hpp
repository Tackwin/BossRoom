#pragma once
#include <type_traits>
#include <variant>

#include "Common.hpp"

template<typename T>
struct OrderedPair {
	constexpr OrderedPair() = default;
	constexpr OrderedPair(const OrderedPair<T>& x) = default;
	constexpr OrderedPair(OrderedPair<T>&& x) = default;
	constexpr OrderedPair(T&& t, T&& u) noexcept : a(t), b(u) {
		if (t > u) { a = u; b = t; }
	};

	constexpr OrderedPair& operator=(const OrderedPair<T>& x) = default;
	constexpr OrderedPair& operator=(OrderedPair<T>&& x) = default;

	constexpr bool operator==(const OrderedPair<T>& other) const noexcept {
		return a == other.a && b == other.b;
	}
	constexpr bool operator<(const OrderedPair<T>& other) const noexcept {
		return a < other.a && b < other.b;
	}

	T a;
	T b;
};

namespace std {
	template<typename T>
	struct hash<OrderedPair<T>> {
		size_t operator()(const OrderedPair<T>& p) const noexcept {
			return xstd::hash_combine(std::hash<T>{}(p.a), p.b);
		}
	};
}