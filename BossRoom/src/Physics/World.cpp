#include <Physics/World.hpp>


void World::update(float dt) {
	for (uint32_t i = 0u; i < _objects.size(); ++i) { //O(n²) BAD VERY BAD
													  //maybe having multiple pool of object "selon(fr)" the ID MASK.
													  //ie: the projectiles don't have to test against each other
		auto& o = _objects[i];

		auto dV = o->force * dt;
		auto dP = o->velocity * dt;

		for (uint32_t j = 0u; j < _objects.size(); ++j) {
			auto& m = _objects[i];

			if ((o->collisionMask & m->idMask).any() && o->collideWith(m)) {
				dV = Vector2::ZERO;
				dP = Vector2::ZERO;

				m->velocity = Vector2::ZERO;
				m->force = Vector2::ZERO;
			}
		}

		o->force = Vector2::ZERO;
		o->velocity += dV;
		o->pos += dP;
	}
}


bool World::haveObject(const Object* obj) const {
	return std::find(_objects.begin(), _objects.end(), obj) != _objects.end();
}
void World::addObject(Object* obj) {
	if (!haveObject(obj))
		_objects.push_back(obj);
}
void World::removeObject(const Object* obj) {
	if (haveObject(obj))
		_objects.erase(std::find(_objects.begin(), _objects.end(), obj));
}
