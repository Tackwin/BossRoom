#pragma once

#include <bitset>
#include <vector>

#include "Const.hpp"

#include "Math/Vector.hpp"
#include "Physics/Collider.hpp"

struct Object {
	enum class BIT_TAGS : u08 {
		PLAYER = 0u,
		FLOOR,
		PROJECTILE,
		BOSS,
		ZONE
	};

	static u64 N;

	static constexpr u08 BITSET_SIZE = 8u;

	Object();

	Vector2f pos = { 0, 0 };

	Vector2f velocity = { 0, 0 };
	std::vector<Vector2f> flatVelocities;
	
	Vector2f force = { 0, 0 };
	std::vector<Vector2f> flatForces;

	std::bitset<BITSET_SIZE> collisionMask = {};
	std::bitset<BITSET_SIZE> idMask = {};

	Collider* collider = nullptr;

	u64 id;
};