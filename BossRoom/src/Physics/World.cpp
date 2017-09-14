#include <Physics/World.hpp>


void World::update(float dt) {
	for (uint32_t i = 0u; i < _objects.size(); ++i) {
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
