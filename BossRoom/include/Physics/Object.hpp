#pragma once

#include <bitset>
#include <Math/Vector2.hpp>
#include <Physics/Collider.hpp>

struct Object {
	static constexpr size_t BITSET_SIZE = 32u;

	Vector2 pos;
	Vector2 velocity;
	Vector2 force;

	std::bitset<BITSET_SIZE> collisionMask;
	std::bitset<BITSET_SIZE> idMask;

	Collider* collider = nullptr;
};