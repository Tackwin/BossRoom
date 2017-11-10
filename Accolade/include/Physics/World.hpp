#pragma once

//Everything here is juste shit, i'm coming back in the old way

#include <map>
#include <memory>
#include <vector>
#include <unordered_map>

#include <SFML/Graphics.hpp>

#include "Common.hpp"

#include "Math/Vector.hpp"

#include "Physics/Object.hpp"

class World {
public:
	void update(double dt);
	void updateInc(double dt, u32 itLevel);
	void render(sf::RenderTarget& target);

	void addObject(std::weak_ptr<Object> obj);
	void delObject(std::weak_ptr<Object> obj);

	void purge();
private:

	void buildUnionCache();
	void removeNeeded();

	std::vector<u32> getUnionOfMask(const std::bitset<Object::BITSET_SIZE>& mask);

	u32 getUID() const;

	u32 _iterationLevel = 5;

	std::unordered_map<u32, std::unordered_map<u32, u08>> _collisionStates;

	std::unordered_map<u32, std::vector<u32>> _unionsCache;
	std::unordered_map<u32, std::weak_ptr<Object>> _objectsMap;

	std::vector<u32> _objectsPool[Object::BITSET_SIZE];
};