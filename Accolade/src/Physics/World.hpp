#pragma once

//Everything here is juste shit, i'm coming back in the old way

#include <map>
#include <memory>
#include <vector>
#include <unordered_map>

#include <SFML/Graphics.hpp>

#include "./../Common.hpp"

#include "./../Math/Vector.hpp"

#include "./../Physics/Object.hpp"
#include "./../Utils/UUID.hpp"

class World {
public:
	void update(double dt);
	void updateInc(double dt, u32 itLevel);
	void render(sf::RenderTarget& target) const noexcept;

	void addObject(std::weak_ptr<Object> obj) noexcept;
	void addObjects(const std::vector<std::weak_ptr<Object>>& objs) noexcept;
	void delObject(UUID uuid);

	void purge();
private:

	void buildUnionCache();
	void removeNeeded();

	std::vector<UUID> getUnionOfMask(const std::bitset<Object::SIZE>& mask);

	u32 _iterationLevel = 5;

	std::unordered_map<UUID, std::unordered_map<UUID, bool>> _collisionStates;

	std::unordered_map<UUID, std::vector<UUID>> _unionsCache;
	std::unordered_map<UUID, std::weak_ptr<Object>> _objectsMap;

	std::vector<UUID> _objectsPool[Object::SIZE];
};