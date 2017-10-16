#pragma once

#include <bitset>
#include <vector>

#include "Math/Vector.hpp"
#include "Physics/Collider.hpp"

struct Object {
	struct BIT_TAGS {
		using type = uint8_t;

		static constexpr type PLAYER = 1;
		static constexpr type FLOOR = PLAYER << 1;
		static constexpr type PROJECTILE = FLOOR << 1;
		static constexpr type BOSS = PROJECTILE << 1;
		static constexpr type ZONE = BOSS << 1;
	};

	static uint64_t N;

	static constexpr size_t BITSET_SIZE = 8u;

	Object();
	virtual ~Object();

	Vector2f pos = { 0, 0 };

	Vector2f velocity = { 0, 0 };
	std::vector<Vector2f> flatVelocities;
	
	Vector2f force = { 0, 0 };
	std::vector<Vector2f> flatForces;

	std::bitset<BITSET_SIZE> collisionMask = 0;
	std::bitset<BITSET_SIZE> idMask;

	Collider* collider = nullptr;

	uint64_t id;
};