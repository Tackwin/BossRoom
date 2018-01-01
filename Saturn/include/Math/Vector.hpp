#pragma once

#include <type_traits>

template<size_t D, typename T>
struct __vec_member {
	T components[D];
};
template<typename T>
struct __vec_member<1, T> {
	union {
		struct {
			T x;
		};
		T components[1];
	};
};
template<typename T>
struct __vec_member<2, T> {
	union {
		struct {
			T x;
			T y;
		};
		T components[2];
	};

	__vec_member() : x(0), y(0) {}
	__vec_member(T x, T y) : x(x), y(y) {}
};
template<typename T>
struct __vec_member<3, T> {
	union {
		struct {
			T x;
			T y;
			T z;
		};
		T components[3];
	};
	__vec_member() : x(0), y(0), z(0) {}
	__vec_member(T x, T y, T z) : x(x), y(y), z(z) {}
};
template<typename T>
struct __vec_member<4, T> {
	union {
		struct {
			T x;
			T y;
			T z;
			T w;
		};
		struct {
			T r;
			T g;
			T b;
			T a;
		};
		T components[4];
	};

	__vec_member() : x(0), y(0), z(0), w(0) {}
	__vec_member(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
};


template<size_t D, typename T = float>
struct Vector : public __vec_member<D, T> {

	static Vector<D, T> createUnitVector(float angles[D]) {
		Vector<D, T> result;
		result[0] = cosf(angles[0]);
		for (size_t i = 1u; i < D; ++i) {

			result[i] = (i + 1u != D) ?
				cosf(angles[i]) :
				1;

			for (size_t j = 0u; j < D - 1u; ++j) {
				result[i] *= sinf(angles[j]);
			}
		}
		return result;
	}
	static Vector<2, T> createUnitVector(float angles) { // i'm not doing all the shit above for 2d
		return { cosf(angles), sinf(angles) };
	}
	static Vector<D, T> createUnitVector(double angles[D]) {
		Vector<D, T> result;
		result[0] = static_cast<T>(cos(angles[0]));
		for (size_t i = 1u; i < D; ++i) {

			result[i] = (i + 1u != D) ?
				static_cast<T>(cos(angles[i])) :
				1;

			for (size_t j = 0u; j < D - 1u) {
				result[i] *= static_cast<T>(sin(angles[j]));
			}
		}
		return result;
	}
	static Vector<2, T> createUnitVector(double angles) { // i'm not doing all the shit above for 2d
		return {
			static_cast<T>(cos(angles)),
			static_cast<T>(sin(angles))
		};
	}
	template<typename V>
	static bool equal(const V& A, const V& B, float eps = FLT_EPSILON) {
		return (A - B).length2() < eps * eps;
	}
	static Vector<D, T> clamp(const Vector<D, T>& V, const Vector<D, T>& min, const Vector<D, T>& max) {
		Vector<D, T> result;
		for (u32 i = 0u; i < D; ++i) {
			result[i] = std::clamp(V[i], min[i], max[i]);
		}
		return result;
	}

	Vector() {
		for (size_t i = 0u; i < D; ++i) {
			components[i] = static_cast<T>(0);
		}
	}

	template<size_t Dp = D>
	Vector(T x, std::enable_if_t<Dp == 2, T> y) :
		__vec_member<2, T>(x, y)
	{}

	template<size_t Dp = D>
	Vector(T x, T y, std::enable_if_t<Dp == 3, T> z) :
		__vec_member<3, T>(x, y, z)
	{}

	template<size_t Dp = D>
	Vector(T x, T y, T z, std::enable_if_t<Dp == 4, T> w) :
		__vec_member<4, T>(x, y, z, w)
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

	Vector<D, T>& clamp(const Vector<D, T>& min, const Vector<D, T>& max) {
		for (u32 i = 0u; i < D; ++i) {
			components[i] = std::clamp(components[i], min[i], max[i]);
		}
		return *this;
	}

	template<typename U>
	bool inRect(const Vector<D, U>& pos, const Vector<D, U>& size) const {
		for (size_t i = 0u; i < D; ++i) {
			if (!(
				static_cast<T>(pos[i]) < components[i] &&
				components[i] < static_cast<T>(pos[i] + size[i])
				)) {
				return false;
			}
		}

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
	std::enable_if_t<Dp == 2, double> angleX() const {
		return std::atan2(y, x);
	}

	template<typename U, size_t Dp = D>
	std::enable_if<Dp == 2, double> angleTo(const Vector<2U, U>& other) const {
		return std::atan2(other.y - y, other.x - x);
	}

	Vector<D, T>& normalize() {
		const auto& l = length();
		if (l == 0) return *this;
		for (size_t i = 0u; i < D; ++i) {
			components[i] /= l;
		}
		return *this;
	}

	void print() {
		for (size_t i = 0u; i < D; ++i) {
			std::cout << components[i];
			if (i != D - 1)
				std::cout << ", ";
		}
		std::cout << std::endl;
	}

	// this is standard notation for scalar product
	// no i don't abuse operator overloading. fuck you
	template<typename U>
	T operator|(const Vector<D, U>& other) const {
		T result = 0;
		for (size_t i = 0u; i < D; ++i) {
			result += components[i] * other[i];
		}
		return result;
	}

	// and this for cross product
	// yeah i know but the syntax sugar is soooooo sweet
	template<typename U, size_t Dp = D>
	std::enable_if_t<Dp == 3, Vector<3U, T>> 
		operator^(const Vector<D, U>& other) const {
		return {
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		};
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
			components[i] *= static_cast<T>(scalaire);
		}
		return *this;
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

	//SFML compatibility stuff
#ifdef SFML_VECTOR2_HPP
	template<typename U, size_t Dp = D>
	Vector(sf::Vector2<U> p, std::enable_if_t<Dp == 2, T>* = nullptr) :
		__vec_member<2, T>(p.x, p.y)
	{}

	template<typename U>
	operator const sf::Vector2<U>() const {
		static_assert(D == 2);
		return {
			static_cast<U>(x),
			static_cast<U>(y)
		};
	}

	template<typename U>
	Vector<D, T>& operator=(const sf::Vector2<U>& other) {
		static_assert(D == 2);
		x = static_cast<T>(other.x);
		y = static_cast<T>(other.y);
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
#endif
};
using Vector2f = Vector<2U, float>;
using Vector2u = Vector<2U, unsigned>;
using Vector2i = Vector<2U, int>;
using Vector4f = Vector<4U, float>;
using Vector4u = Vector<4U, unsigned>;
using Vector4i = Vector<4U, int>;

template<size_t D, typename T, typename U>
Vector<D, T> operator*(U scalar, const Vector<D, T>& vec) {
	return vec * scalar;
}

#define COLOR_UNROLL(x) x.r, x.g, x.b, x.a
#define XYZW_UNROLL(v) v.x, v.y, v.z, v.w
