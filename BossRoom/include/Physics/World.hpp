#pragma once

//Everything here is juste shit, i'm coming back in the old way

#include <array>
#include <vector>

#include <Math/Vector2.hpp>
#include <Physics/Object.hpp>

class World {
public:
	void update(float dt);


	bool haveObject(const Object* obj) const;
	void addObject(Object* obj);
	void removeObject(const Object* obj);

private:

	std::vector<Object*> _objects;
	std::array<std::vector<Object*>, Object::BITSET_SIZE> _objectsPool;
	Vector2 _gravity;
};