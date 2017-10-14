#pragma once

#include <type_traits>

#include <SFML/System/Vector2.hpp>


template<size_t D, typename T = float>
struct Vector {
	static Vector<D, T> createUnitVector(float angles[D]) {
		Vector<D, T> result;
		result[0] = cosf(angles[0]);
		for (size_t i = 1u; i < D; ++i) {

			result[i] = (i + 1u != D) ? 
							cosf(angles[i]) : 
							1;

			for (size_t j = 0u; j < D - 1u) {
				result[i] *= sinf(angles[j]);
			}
		}
		return result;
	}
	static Vector<2, T> createUnitVector(float angles) { // i'm not doing all the shit above for 2d
		return { cosf(angles), sinf(angles) };
	}

	template<typename V>
	static bool equal(const V& A, const V& B, float eps = FLT_EPSILON) {
		return (A - B).length2() < eps * eps;
	}

	union {
		struct {
			T x;
			T y;
			T z;
		};
		T components[D];
	};

	Vector() {}
	
	template<size_t Dp = D, typename U>
	Vector(const sf::Vector2<U>& vec, typename std::enable_if_t<Dp == 2, T>* = nullptr) :
		x(vec.x),
		y(vec.y) 
	{}

	template<size_t Dp = D>
	Vector(T x = 0, T y = 0, typename std::enable_if_t<Dp == 2, T>* = nullptr) :
		x(x),
		y(y)
	{}

	size_t getDimension() const {
		return D;
	}

	T& operator[](size_t i) {
		return components[i];
	}
	const T& operator[](size_t i) const {
		return components[i];
	}

	bool inRect(const Vector<D, T>& pos, const Vector<D, T>& size) const {
		for (size_t i = 0u; i < D; ++i)
			if (!(pos[i] < components[i] && components[i] < pos[i] + size[i]))
				return false;

		return true;
	}

	T length() const {
		T result = 0;
		for (size_t i = 0u; i < D; ++i) {
			result += components[i] * components[i];
		}
		return sqrt(result);
	}

	T length2() const {
		T result = 0;
		for (size_t i = 0u; i < D; ++i) {
			result += components[i] * components[i];
		}
		return result;
	}

	template<size_t Dp = D>
	std::enable_if_t<D == 2, double> angleX() const {
		return std::atan2(y, x);
	}

	template<typename U, size_t Dp = D>
	std::enable_if_t<D == 2, double> angleTo(const Vector<2U, U>& other) const {
		return std::atan2(other.y - y, other.x - x);
	}

	Vector<D, T>& normalize() {
		const auto& l = length();
		for (size_t i = 0u; i < D; ++i) {
			components[i] /= l;
		}
		return *this;
	}



	template<typename U>
	Vector<D, T> operator*(const U& scalaire) const {
		static_assert(std::is_scalar<U>::value, "need to be a scalar");
		Vector<D, T> result;
		for (size_t i = 0; i < getDimension(); ++i) {
			result[i] = static_cast<T>(components[i] * scalaire);
		}

		return result;
	}
	template<typename U>
	Vector<D, T> operator/(const U& scalaire) const {
		static_assert(std::is_scalar<U>::value);
		Vector<D, T> result;
		for (size_t i = 0; i < getDimension(); ++i) {
			result[i] = static_cast<T>(components[i] / scalaire);
		}

		return result;
	}

	template<typename U>
	Vector<D, T> operator+(const U& other) const {
		Vector<D, T> result;
		for (size_t i = 0; i < getDimension(); ++i) {
			result[i] = static_cast<T>(components[i] + other[i]);
		}

		return result;
	}
	template<typename U>
	Vector<D, T> operator-(const U& other) const {
		Vector<D, T> result;
		for (size_t i = 0; i < getDimension(); ++i) {
			result[i] = static_cast<T>(components[i] - other[i]);
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
	Vector<D, T>& operator+=(const U& other) {
		for (size_t i = 0; i < getDimension(); ++i) {
			components[i] += static_cast<T>(other[i]);
		}
		return *this;
	}
	template<typename U>
	Vector<D, T>& operator*=(const U& scalaire) {
		static_assert(std::is_scalar<U>::value);
		for (size_t i = 0; i < getDimension(); ++i) {
			components[i] += static_cast<T>(scalaire);
		}
		return *this;
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
