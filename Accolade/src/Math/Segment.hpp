#pragma once

#include "Vector.hpp"

template<typename T>
struct Segment2 {

	Vector2<T> A;
	Vector2<T> B;

	Segment2(Vector2<T> A = Vector2<T>(), Vector2<T> B = Vector2<T>()) : A(A), B(B) {}

	bool intersect(Segment2<T> other) {
		T s1_x;
		T s1_y;
		T s2_x;
		T s2_y;

		s1_x = B.x - A.x;
		s1_y = B.y - A.y;
		s2_x = other.B.x - other.A.x;
		s2_y = other.B.y - other.A.y;

		float s, t;
		s = (-s1_y * (A.x - other.A.x) + s1_x * (A.y - other.A.y)) / (-s2_x * s1_y + s1_x * s2_y);
		t = (s2_x * (A.y - other.A.y) - s2_y * (A.x - other.A.x)) / (-s2_x * s1_y + s1_x * s2_y);

		return s >= 0 && s <= 1 && t >= 0 && t <= 1;
	}

};