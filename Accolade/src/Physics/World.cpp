#include "World.hpp"

#include <algorithm>
#include <numeric>

void World::updateInc(double dt, u32 itLevel) {
	for (u32 i = 0u; i < itLevel; ++i) {
		update(dt / itLevel);
	}

	for (auto& [_, wobj1] : _objectsMap) {_;
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

		Vector2f flatForces = std::accumulate(
			obj1->flatForces.begin(), 
			obj1->flatForces.end(),
			Vector2f(0, 0)
		);
		Vector2f flatVelocities = std::accumulate(
			obj1->flatVelocities.begin(), 
			obj1->flatVelocities.end(), 
			Vector2f(0, 0)
		);

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

				if (obj1->collider->collideWith(obj2->collider.get())) {
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

				if (obj1->collider->collideWith(obj2->collider.get())) {
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
	for (auto& [_, obj] : _objectsMap) {_;
		if (obj.expired()) continue;

		obj.lock()->collider->render(target);
	}
}

void World::addObject(std::weak_ptr<Object> obj) {
	auto id = obj.lock()->uuid;
	_objectsMap[id] = obj;

	for (u32 i = 0u; i < Object::SIZE; ++i) {
		if (!obj.lock()->idMask[i]) continue;

		_objectsPool[i].push_back(id);
		std::sort(_objectsPool[i].begin(), _objectsPool[i].end());
	}

	buildUnionCache();
}

void World::delObject(UUID uuid) {
	using map_pair = std::pair<UUID, std::weak_ptr<Object>>;
	auto object_map_it = std::find_if(
		_objectsMap.begin(),
		_objectsMap.end(),
		[uuid](const map_pair& A) -> bool {
			return A.first == uuid;
		}
	);

	assert(object_map_it != _objectsMap.end() && "Object to delete isn't present.");

	auto id = object_map_it->first;

	for (size_t i = 0u; i < Object::SIZE; ++i) {
		if (!object_map_it->second.lock()->idMask[i]) continue;

		auto& object_pool_it = 
			std::find(_objectsPool[i].cbegin(), _objectsPool[i].cend(), id);

		_objectsPool[i].erase(object_pool_it);
		std::sort(_objectsPool[i].begin(), _objectsPool[i].end());
	}

	_collisionStates.erase(id);
	for (auto& c : _collisionStates) {
		c.second.erase(id);
	}
	_objectsMap.erase(object_map_it);

	buildUnionCache();
}

void World::removeNeeded() {
	std::vector<UUID> toRemove;

	for (auto& it : _objectsMap) {
		auto& id = it.first;
		auto& obj = it.second;

		if (!obj.expired()) {
			continue;
		}

		for (u32 j = 0u; j < Object::SIZE; ++j) {
			auto& jt = 
				std::find(_objectsPool[j].cbegin(), _objectsPool[j].cend(), id);
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
	for (u32 i = 0u; i < Object::SIZE; ++i)
		_objectsPool[i].clear();
}

std::vector<UUID>
	World::getUnionOfMask(const std::bitset<Object::SIZE>& mask)
{
	std::vector<UUID> result;
	result.reserve(_objectsMap.size() * Object::SIZE);

	for (size_t i = 0u; i < Object::SIZE; ++i) {
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
