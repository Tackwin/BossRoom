#pragma once

#include <type_traits>

#include <SFML/System/Vector2.hpp>

template<size_t D, typename T = float>
struct Vector {
	union {
		struct {
			T x;
			T y;
			T z;
		};
		T components[D];
	};

	size_t getDimension() const {
		return D;
	}

	T& operator[](size_t i) {
		return components[i];
	}

	template<typename U>
	Vector operator*(const U& scalaire) {
		Vector<D, T> result;
		for (size_t i = 0; i < getDimension(); ++i) {
			result[i] = components[i] * scalaire;
		}

		return result;
	}

	template<typename U>
	Vector operator+(const U& scalaire) {
		Vector<D, T> result;
		for (size_t i = 0; i < getDimension(); ++i) {
			result[i] = components[i] + scalaire;
		}

		return result;
	}

	template<typename U>
	operator const sf::Vector2<U>() const {
		return {
			static_cast<U>(x), 
			static_cast<U>(y)
		};
	}

	template<typename U>
	bool operator==(const sf::Vector2<U>& other) const {
		static_assert(D == 2);
		return components[0] == other.x && components[1] == other.y;
	}
	template<typename U>
	bool operator!=(const sf::Vector2<U>& other) const {
		static_assert(D == 2);
		return !(components[0] == other.x && components[1] == other.y);
	}
	template<typename U>
	bool operator==(const Vector<D, U>& other) const {
		for (size_t i = 0u; i < D; ++i) {
			if (components[i] != other.components[i])
				return false;
		}
		return true;
	}
	template<typename U>
	bool operator!=(const Vector<D, U>& other) const {
		return !(this->operator==(other));
	}
};

using Vector2f = Vector<2U, float>;