#include "algorithms.hpp"

bool is_in_ellipse(Vector2f A, Vector2f B, float small_axis, Vector2f p) noexcept {
	return	(p - A).length2() + (p - B).length2() <=
			4 * ((B - A).length2() + small_axis * small_axis);
}
