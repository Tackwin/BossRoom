#pragma once
#include <cstdint>

// We assume that the axis are x left to right, y up to down
enum class Dir : std::uint8_t {
	Right = 0,
	Bot,
	Left,
	Top
};

extern Dir dir_from_rad(float rad) noexcept;
extern Dir dir_from_rad(double rad) noexcept;
extern Dir dir_complementary(Dir d) noexcept;

