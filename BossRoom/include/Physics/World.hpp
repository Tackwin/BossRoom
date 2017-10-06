#pragma once

//Everything here is juste shit, i'm coming back in the old way

#include <map>
#include <vector>

#include <SFML/Graphics.hpp>

#include <Math/Vector2.hpp>
#include <Physics/Object.hpp>

#include <Graphics/Particle.hpp>

#include <Gameplay/Player.hpp>
#include <Gameplay/Projectile.hpp>
#include <Gameplay/Zone.hpp>
#include <Gameplay/Boss.hpp>
class World {
public:
	void update(float dt);
	void updateInc(float dt, uint32_t itLevel);
	void render(sf::RenderTarget& target);

	void addObject(std::weak_ptr<Object> obj);
	void delObject(std::weak_ptr<Object> obj);

	void purge();
private:
	template<typename T>
	struct ordered_pair {
		T first;
		T second;

		ordered_pair(T a, T b) {
			first = b;
			second = a;
			if (a < b) {
				first = a;
				second = b;
			}
		}

		bool operator==(const ordered_pair<T>& other) const {
			return first == other.first && second == other.second;
		}
		bool operator<(const ordered_pair<T>& other) const {
			return first < other.first && second < other.second;
		}
	};

	uint32_t _iterationLevel = 5;

	std::map<ordered_pair<uint64_t>, bool> _collisionStates;

	std::vector<std::weak_ptr<Object>> _objects;
	std::vector<std::weak_ptr<Object>> _objectsPool[Object::BITSET_SIZE];
};