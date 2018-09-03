#pragma once

#include "Math/Vector.hpp"

struct Bumpable {
	virtual void bump(Vector2f force) noexcept = 0;
};