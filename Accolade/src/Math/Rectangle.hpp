#pragma once

#include "Vector.hpp"

template<typename T>
struct Rectangle {
	union {
		struct {
			Vector<2, T> pos;
			Vector<2, T> size;
		};
		struct {
			T x;
			T y;
			T w;
			T h;
		};
	};

	Rectangle(const Vector<2, T>& pos, const Vector<2, T>& size) :
		pos(pos),
		size(size) 
	{}

	bool intersect(const Rectangle<T>& other) const {
		return !(
				pos.x + size.x < other.pos.x || pos.x > other.pos.x + other.size.x ||
				pos.y + size.y < other.pos.y || pos.y > other.pos.y + other.size.y
			);
	}

	Vector<2, T> center() const {
		return pos + size / 2;
	}

	T bot() const {
		return pos.y + size.y;
	}

	template<typename U>
	bool in(const Vector<2, U>& p) const {
		return p.inRect(pos, size);
	}
};

template<typename T>
using Rectangle2 = Rectangle<T>;
using Rectangle2f = Rectangle2<float>;