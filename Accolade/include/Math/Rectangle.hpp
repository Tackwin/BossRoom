#pragma once

#include "Vector.hpp"

template<size_t D, typename T>
struct Rectangle {
	union {
		struct {
			Vector<D, T> pos;
			Vector<D, T> size;
		};
		struct {
			T x;
			T y;
			T w;
			T h;
		};
	};

	Rectangle(const Vector<D, T>& pos, const Vector<D, T>& size) :
		pos(pos),
		size(size) 
	{}

	bool intersect(const Rectangle<D, T>& other) const {
		return !(
				pos.x + size.x < other.pos.x || pos.x > other.pos.x + other.size.x ||
				pos.y + size.y < other.pos.y || pos.y > other.pos.y + other.size.y
			);
	}
};