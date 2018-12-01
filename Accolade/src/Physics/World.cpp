#include "World.hpp"

#include <unordered_set>
#include <algorithm>
#include <numeric>

void World::updateInc(double dt, u32 itLevel) {
	removeNeeded();

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

	for (auto& [id, weak_obj] : _objectsMap) {
		if (weak_obj.expired()) continue;
		auto obj = weak_obj.lock();

		Vector2f flatForces = std::accumulate(
			obj->flatForces.begin(),
			obj->flatForces.end(),
			Vector2f(0, 0)
		);
		Vector2f flatVelocities = std::accumulate(
			obj->flatVelocities.begin(),
			obj->flatVelocities.end(),
			Vector2f(0, 0)
		);

		Vector2f pos = obj->pos;

		Vector2f new_vel = obj->velocity + (obj->force + flatForces) * dt;
		Vector2f nPos = obj->pos + (new_vel + flatVelocities) * dt * 0.5f;

		// We set twice the velocity beacause the user callback might want to know
		// this info.
		obj->velocity = new_vel;

		bool xCollider = false;
		bool yCollider = false;

		auto& collisionState = _collisionStates[id];

		for (auto&[id2, weak_obj2] : _objectsMap) {
			if (id == id2) continue;
			if (weak_obj2.expired()) continue;

			auto obj2 = weak_obj2.lock();
			if ((obj->collisionMask & obj2->idMask).none()) continue;

			bool flag = false;
			if (!xCollider) {
				obj->pos.x = nPos.x;
				obj->collider->setPos({ nPos.x, pos.y });

				if (
					obj->collider->collideWith(obj2->collider.get()) &&
					obj->collider->filterCollide(*obj2.get())
				) {
					if (!obj->collider->sensor) {
						new_vel.x = 0;
						obj->pos.x = pos.x;
						nPos.x = pos.x;
						xCollider = true;
					}

					flag = true;
				}
			}

			if (!yCollider) {
				obj->pos.y = nPos.y;
				obj->collider->setPos({ pos.x, nPos.y });

				if (
					obj->collider->collideWith(obj2->collider.get())
				) {
					if (obj->collider->filterCollide(*obj2.get())) {
						if (!obj->collider->sensor) {
							new_vel.y = 0;
							obj->pos.y = pos.y;
							nPos.y = pos.y;
							yCollider = true;
						}

						flag = true;
					}
				}
			}

			auto& collisionStateRef = collisionState[id2];

			if (!flag && collisionStateRef) {
				obj->collider->onExit(obj2.get());
				//obj2->collider->onExit(obj1.get());
			}
			if (flag && !collisionStateRef) {
				obj->collider->onEnter(obj2.get());
				//obj2->collider->onEnter(obj1.get());
			}
			collisionStateRef = flag;

			if (xCollider && yCollider) {
				break;
			}
		}

		obj->force = { 0, 0 };
		obj->velocity = new_vel;
		obj->pos = nPos;

		if (obj->collider)
			obj->collider->setPos(nPos);
	}
}

void World::render(sf::RenderTarget& target) const noexcept {
	for (auto& [_, obj] : _objectsMap) {_;
		if (obj.expired()) continue;

		if (obj.lock()->collider)
			obj.lock()->collider->render(target);
	}
}

void World::addObject(std::weak_ptr<Object> obj) noexcept {
	auto id = obj.lock()->uuid;
	_objectsMap[id] = obj;

	for (u32 i = 0u; i < Object::SIZE; ++i) {
		if (!obj.lock()->idMask[i]) continue;

		_objectsPool[i].push_back(id);
		std::sort(_objectsPool[i].begin(), _objectsPool[i].end());
	}

	buildUnionCache();
}

void World::addObjects(const std::vector<std::weak_ptr<Object>>& objs) noexcept {
	for (auto& o : objs) {
		auto id = o.lock()->uuid;

		_objectsMap[id] = o;

		for (u32 i = 0; i < Object::SIZE; ++i) {
			if (!o.lock()->idMask[i]) continue;

			_objectsPool[i].push_back(id);
		}
	}

	for (auto& pool : _objectsPool) {
		std::sort(std::begin(pool), std::end(pool));
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

	for (auto& [id, obj] : _objectsMap) {

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
	//buildUnionCache();
}
#pragma warning(pop)

void World::purge() {
	_objectsMap.clear();
	_unionsCache.clear();
	_objectsMap.clear();
	_collisionStates.clear();
	for (u32 i = 0u; i < Object::SIZE; ++i)
		_objectsPool[i].clear();
}

std::vector<UUID>
	World::getUnionOfMask(const std::bitset<Object::SIZE>& mask)
{
	static std::unordered_set<UUID> result;
	result.clear();

	for (size_t i = 0u; i < Object::SIZE; ++i) {
		if (!mask[i]) continue;

		result.insert(std::begin(_objectsPool[i]), std::end(_objectsPool[i]));
	}

	return std::vector(std::begin(result), std::end(result));
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

std::weak_ptr<Object> World::getObject(UUID id) const noexcept {
	if (auto it = _objectsMap.find(id); it != _objectsMap.end()) {
		return it->second;
	}

	return {};
}
