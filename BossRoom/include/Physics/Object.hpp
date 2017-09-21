#pragma once

#include <bitset>
#include <vector>

#include <Math/Vector2.hpp>
#include <Physics/Collider.hpp>

struct Object {
	enum BIT_TAGS {
		PLAYER = 1,
		FLOOR = PLAYER << 1,
		PROJECTILE = FLOOR << 1,
		BOSS = PROJECTILE << 1
	};

	static uint64_t N;

	static constexpr size_t BITSET_SIZE = 8u;

	Object();

	Vector2 pos = Vector2::ZERO;

	Vector2 velocity = Vector2::ZERO;
	std::vector<Vector2> flatVelocities;
	
	Vector2 force = Vector2::ZERO;
	std::vector<Vector2> flatForces;

	std::bitset<BITSET_SIZE> collisionMask = 0;
	std::bitset<BITSET_SIZE> idMask;

	Collider* collider = nullptr;

	uint64_t id;
};