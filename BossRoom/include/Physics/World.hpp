#pragma once

#include <Math/Vector2.hpp>
#include <Physics/Object.hpp>

class World {
public:
	void update(float dt);

	void addObject(const Object);

	std::vector<Object*> _objects;
private:


	Vector2 _gravity;
};