#pragma once

#include <array>

template<size_t D, typename T = float>
struct Vector {
	std::array<T, D> components;

	T& operator[](size_t i) {
		return components[i];
	}
};

template<typename T>
struct Vector<2U, T> {
	T x;
	T y;
};

using Vector2f = Vector<2U, float>;