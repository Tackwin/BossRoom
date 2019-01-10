#include "Dir.hpp"
#include <cmath>
#include <cassert>
#include <cstdio>

static constexpr auto PI = 3.1415926;

Dir dir_from_rad(float rad) noexcept {
	if (std::fabs(rad) > 3.f * PI / 4.f) return Dir::Left;
	if (std::fabs(rad) < PI / 4.f) return Dir::Right;
	if (rad > 0) return Dir::Bot;
	return Dir::Top;
}
Dir dir_from_rad(double rad) noexcept {
	if (std::abs(rad) > 3.f * PI / 4.f) return Dir::Left;
	if (std::abs(rad) < PI / 4.f) return Dir::Right;
	if (rad > 0) return Dir::Bot;
	return Dir::Top;
}

Dir dir_complementary(Dir d) noexcept {
	switch (d)
	{
	case Dir::Right:
		return Dir::Left;
		break;
	case Dir::Bot:
		return Dir::Top;
		break;
	case Dir::Left:
		return Dir::Right;
		break;
	case Dir::Top:
		return Dir::Bot;
		break;
	default:
		assert("Did you add a cardinal direction ?");
		return d;
		break;
	}
}

