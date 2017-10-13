#pragma once

#include <bitset>
#include <vector>

#include <Math/Vec.hpp>
#include <Physics/Collider.hpp>

struct Object {
	enum BIT_TAGS {
		PLAYER = 1,
		FLOOR = PLAYER << 1,
		PROJECTILE = FLOOR << 1,
		BOSS = PROJECTILE << 1,
		ZONE = BOSS << 1
	};

	static uint64_t N;

	static constexpr size_t BITSET_SIZE = 8u;

	Object();
	virtual ~Object();

	Vector2f pos = Vector2f::ZEROf;

	Vector2f velocity = Vector2f::ZEROf;
	std::vector<Vector2f> flatVelocities;
	
	Vector2f force = Vector2f::ZEROf;
	std::vector<Vector2f> flatForces;

	std::bitset<BITSET_SIZE> collisionMask = 0;
	std::bitset<BITSET_SIZE> idMask;

	Collider* collider = nullptr;

	uint64_t id;
};