#include "algorithms.hpp"

bool is_in_ellipse(Vector2f A, Vector2f B, float small_axis, Vector2f p) noexcept {
	return	(p - A).length() + (p - B).length() <=
			(B - A).length() + small_axis;
}
