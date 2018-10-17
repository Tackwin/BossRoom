#include "Object.hpp"
#include <algorithm>

u64 Object::N = 0u;

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