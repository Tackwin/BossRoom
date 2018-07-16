#pragma once

#include <bitset>
#include <vector>

#include "./../Common.hpp"

#include "./../Utils/UUID.hpp"

#include "./../Math/Vector.hpp"

#include "./../Physics/Collider.hpp"

struct Object {
	enum BIT_TAGS : u32 {
		PLAYER = 0u,
		FLOOR,
		PROJECTILE,
		BOSS,
		ZONE,
		STRUCTURE,
		SLIME,
		SIZE
	};

	static u64 N;

	Object();


	Vector2f pos = { 0, 0 };

	Vector2f velocity = { 0, 0 };
	std::vector<Vector2f> flatVelocities;
	
	Vector2f force = { 0, 0 };
	std::vector<Vector2f> flatForces;

	std::bitset<SIZE> collisionMask = {};
	std::bitset<SIZE> idMask = {};

	std::unique_ptr<Collider> collider{ nullptr };

	const UUID uuid;
};