#include "Object.hpp"
#include <algorithm>

u64 Object::N = 0u;

std::bitset<Object::SIZE> Object::opaque_mask = xstd::consecutive_to_bitset<Object::SIZE>(
	Object::FLOOR,
	Object::PLAYER,
	Object::STRUCTURE
);

Object::Object() : uuid() {
	N++;
}

Object::~Object() {}

Vector2f get_summed_velocities(Object& object) noexcept {
	return std::accumulate(
		std::begin(object.flatVelocities),
		std::end(object.flatVelocities),
		object.velocity
	);
}