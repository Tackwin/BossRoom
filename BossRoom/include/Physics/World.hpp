#pragma once

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
	Vector2 _gravity;
};