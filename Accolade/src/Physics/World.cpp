#include "Physics/World.hpp"

#include <algorithm>
#include <numeric>

void World::updateInc(double dt, uint32_t itLevel) {
	for (uint32_t i = 0u; i < itLevel; ++i) {
		update(dt / itLevel);
	}

	for (auto& [_, wobj1] : _objectsMap) { // pls fix this intellisense bug soon ;(
		auto obj1 = wobj1.lock();

		obj1->flatVelocities.clear();
		obj1->flatForces.clear();
	}
}

#pragma warning(push)
#pragma warning(disable:4239)
void World::update(double dt) {
	removeNeeded();

	for (auto& it : _objectsMap) {
		auto obj1Id = it.first;
		auto obj1 = it.second.lock();

		Vector2f flatForces = std::accumulate(obj1->flatForces.begin(), obj1->flatForces.end(), Vector2f(0, 0));
		Vector2f flatVelocities = std::accumulate(obj1->flatVelocities.begin(), obj1->flatVelocities.end(), Vector2f(0, 0));

		Vector2f pos = obj1->pos;

		Vector2f nVel = obj1->velocity + (obj1->force + flatForces) * dt;
		Vector2f nPos = obj1->pos + (nVel + flatVelocities) * dt * 0.5f;

		bool xCollider = false;
		bool yCollider = false;

		auto& collisionState = _collisionStates[obj1Id];

		for (auto& obj2Id : _unionsCache[obj1Id]) {
			auto obj2 = _objectsMap[obj2Id].lock();

			if (obj1.get() == obj2.get()) continue;

			bool flag = false;
			if (!xCollider) {
				obj1->pos.x = nPos.x;
				obj1->collider->setPos({ nPos.x, pos.y });

				if (obj1->collider->collideWith(obj2->collider)) {
					if (!obj2->collider->sensor) {
						nVel.x = 0;
						obj1->pos.x = pos.x;
						nPos.x = pos.x;
						xCollider = true;
					}
				
					flag = true;
				}
			}

			if (!yCollider) {
				obj1->pos.y = nPos.y;
				obj1->collider->setPos({ pos.x, nPos.y });

				if (obj1->collider->collideWith(obj2->collider)) {
					if (!obj2->collider->sensor){
						nVel.y = 0;
						obj1->pos.y = pos.y;
						nPos.y = pos.y;
						yCollider = true;
					}

					flag = true;
				}
			}

			auto& collisionStateRef = collisionState[obj2Id];
			if (!flag) {
				if (collisionStateRef) {
					obj1->collider->onExit(obj2.get());
					obj2->collider->onExit(obj1.get());
				}

				collisionStateRef = false;
			}
			else {
				if (!collisionStateRef) {
					obj1->collider->onEnter(obj2.get());
					obj2->collider->onEnter(obj1.get());
				}

				collisionStateRef = true;
			}

			if (xCollider && yCollider) {
				break;
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
	for (auto& [_, obj] : _objectsMap) {
		if (obj.expired()) continue;

		obj.lock()->collider->render(target);
	}
}

void World::addObject(std::weak_ptr<Object> obj) {
	uint32_t id = getUID();
	_objectsMap[id] = obj;

	for (uint32_t i = 0u; i < Object::BITSET_SIZE; ++i) {
		if (!obj.lock()->idMask[i]) continue;

		_objectsPool[i].push_back(id);
		std::sort(_objectsPool[i].begin(), _objectsPool[i].end());
	}

	buildUnionCache();
}

void World::delObject(std::weak_ptr<Object> obj_) {
	uint32_t id = 0u;
	for (auto& [id_, obj] : _objectsMap) {
		if (obj.lock() == obj_.lock()) {
			id = id_;
			_objectsMap.erase(id);
			break;
		}
	}

	for (size_t i = 0u; i < Object::BITSET_SIZE; ++i) {
		if (!obj_.lock()->idMask[i]) continue;

		auto &it = std::find(_objectsPool[i].cbegin(), _objectsPool[i].cend(), id);
		_objectsPool[i].erase(it);
		std::sort(_objectsPool[i].begin(), _objectsPool[i].end());
	}
	buildUnionCache();
}

void World::removeNeeded() {
	std::vector<uint32_t> toRemove;

	for (auto& it : _objectsMap) {
		auto& id = it.first;
		auto& obj = it.second;

		if (!obj.expired()) {
			continue;
		}

		for (uint32_t j = 0u; j < Object::BITSET_SIZE; ++j) {
			auto& jt = std::find(_objectsPool[j].cbegin(), _objectsPool[j].cend(), id);
			if (jt == _objectsPool[j].end()) continue;

			_objectsPool[j].erase(jt);
		}

		toRemove.push_back(id);
	}

	for (auto& id : toRemove) {
		_objectsMap.erase(id);
	}
}
#pragma warning(pop)

void World::purge() {
	_objectsMap.clear();
	for (uint32_t i = 0u; i < Object::BITSET_SIZE; ++i) 
		_objectsPool[i].clear();
}

uint32_t World::getUID() const {
	uint32_t n_id = 0u;
	bool flag = false;
	do {
		flag = false;
		for (auto& [id, _] : _objectsMap) {
			if (n_id == id) {
				n_id++;
				flag = true;
			}
		}
	} while (flag);
	return n_id;
}

std::vector<uint32_t> World::getUnionOfMask(const std::bitset<Object::BITSET_SIZE>& mask) {
	std::vector<uint32_t> result;
	result.reserve(_objectsMap.size() * Object::BITSET_SIZE);

	for (size_t i = 0u; i < Object::BITSET_SIZE; ++i) {
		if (!mask[i]) continue;

		std::set_union(	_objectsPool[i].begin()	, _objectsPool[i].end(),
						result.begin()			, result.end(),
						std::back_inserter(result));
	}

	return result;
}

void World::buildUnionCache() {
	_unionsCache.clear();

	for (auto& it : _objectsMap) {
		auto& id = it.first;
		auto& obj = it.second;

		if (obj.expired()) continue;
		_unionsCache[id] = getUnionOfMask(obj.lock()->collisionMask);
	}
}
