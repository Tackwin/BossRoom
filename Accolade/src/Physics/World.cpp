#include "Physics/World.hpp"

#include <algorithm>
#include <numeric>

void World::updateInc(double dt, uint32_t itLevel) {
	for (uint32_t i = 0u; i < itLevel; ++i) {
		update(dt / itLevel);
	}

	for (auto wobj1 : _objects) {
		auto obj1 = wobj1.lock();

		obj1->flatVelocities.clear();
		obj1->flatForces.clear();
	}
}

void World::update(double dt) {
	for (uint32_t i = _objects.size(); i > 0u; --i) {
		if (!_objects[i - 1].expired()) continue;
		
		_objects.erase(_objects.begin() + i - 1);
	}

	for (uint32_t j = 0u; j < Object::BITSET_SIZE; ++j) {
		for (uint32_t k = _objectsPool[j].size(); k > 0u; --k) {
			if (!_objectsPool[j][k - 1].expired()) continue;
			
			_objectsPool[j].erase(_objectsPool[j].begin() + k - 1);
		}
	}

	for (auto& obj1w : _objects) {
		auto obj1 = obj1w.lock();

		Vector2f flatForces = { 0, 0 };
		for (const auto& v : obj1->flatForces) {
			flatForces += v;
		}
		Vector2f flatVelocities = { 0, 0 };
		for (const auto& v : obj1->flatVelocities) {
			flatVelocities += v;
		}

		Vector2f pos = obj1->pos;

		Vector2f nVel = obj1->velocity + (obj1->force + flatForces) * dt;
		Vector2f nPos = obj1->pos + (nVel + flatVelocities) * dt * 0.5f;

		bool xCollider = false;
		bool yCollider = false;

		bool collisionCalback = false;

		for (uint32_t i = 0u; i < Object::BITSET_SIZE && obj1->collider; ++i) {
			if (!obj1->collisionMask[i]) continue;
			
			for (auto& obj2w : _objectsPool[i]) {
				auto obj2 = obj2w.lock();

				if (obj1.get() == obj2.get()) continue;

				obj1->pos.x = nPos.x;
				obj1->collider->setPos({ nPos.x, pos.y });
				if (!xCollider && obj1->collider->collideWith(obj2->collider)) {
					nVel.x = 0;
					obj1->pos.x = pos.x;
					nPos.x = pos.x;
					xCollider = true;
				}

				obj1->pos = nPos;
				obj1->collider->setPos(nPos);
				if (!yCollider && obj1->collider->collideWith(obj2->collider)) {
					nVel.y = 0;
					obj1->pos.y = pos.y;
					nPos.y = pos.y;
					yCollider = true;
				}

				if ((xCollider || yCollider) && !collisionCalback) {
					if (!_collisionStates[{obj1->id, obj2->id}]) {
						obj1->collider->onEnter(obj2.get());
						obj2->collider->onEnter(obj1.get());
					}

					_collisionStates[{obj1->id, obj2->id}] = true;
					collisionCalback = true;
				}
				else {
					_collisionStates[{obj1->id, obj2->id}] = false;
				}

				if (xCollider && yCollider) {
					break;
				}
			}
		}

		obj1->force = { 0, 0 };
		obj1->velocity = nVel;
		obj1->pos = nPos;

		if (obj1->collider)
			obj1->collider->setPos(nPos);
	}
}

void World::render(sf::RenderTarget& target) {
	for (auto& obj : _objects) {
		if (obj.expired()) continue;

		obj.lock()->collider->render(target);
	}
}


void World::addObject(std::weak_ptr<Object> obj) {
	_objects.push_back(obj);

	for (uint32_t i = 0u; i < Object::BITSET_SIZE; ++i) {
		if (!obj.lock()->idMask[i]) continue;

		_objectsPool[i].push_back(obj);
	}
}

void World::delObject(std::weak_ptr<Object> obj) {
	for (uint32_t i = _objects.size(); i > 0u; --i) {
		if (_objects[i - 1].lock() == obj.lock()) {
			_objects.erase(_objects.begin() + i - 1);
			break;
		}
	}

	for (uint32_t i = 0u; i < Object::BITSET_SIZE; ++i) {
		if (!obj.lock()->idMask[i]) continue;

		for (uint32_t j = _objectsPool[i].size(); j > 0u; --j) {
			if (_objectsPool[i][j - 1].lock() == obj.lock()) {
				_objectsPool[i].erase(_objectsPool[i].begin() + j - 1);
				break;
			}
		}
	}
}

void World::purge() {
	_objects.clear();
	for (uint32_t i = 0u; i < Object::BITSET_SIZE; ++i) 
		_objectsPool[i].clear();
}
