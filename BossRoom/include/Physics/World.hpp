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
	void render(sf::RenderTarget& target);

	void setPlayer(const std::shared_ptr<Player>& player, uint32_t idx);
	void delPlayer(uint32_t idx);

	void setBoss(const std::shared_ptr<Boss>& boss, uint32_t idx);
	void delBoss(uint32_t idx);

	void addZone(const std::shared_ptr<Zone>& zone);

	void addProjectile(const std::shared_ptr<Projectile>& projectile);

	void addFloor(Vector2 vec);
private:

	void burstParticle(const std::shared_ptr<const Boss>& boss, const Vector2& pos);

	void removeNeeded();
	void pullAllProjectiles();
	void updateBosses(float dt);
	void updateProjectiles(float dt);
	void updateParticles(float dt);

	std::vector<Vector2> _floors;
	std::vector<std::weak_ptr<Boss>> _bosses;
	std::vector<std::shared_ptr<Zone>> _zones;
	std::vector<std::weak_ptr<Player>> _players;
	std::vector<std::shared_ptr<Particle>> _particles;
	std::vector<std::shared_ptr<Projectile>>  _projectiles;
};

class WorldExp {
public:
	void update(float dt);
	void updateInc(float dt, uint32_t itLevel);
	void render(sf::RenderTarget& target);

	void addObject(std::weak_ptr<Object> obj);
	void delObject(std::weak_ptr<Object> obj);
private:
	uint32_t _iterationLevel = 5;

	std::map<std::pair<uint64_t, uint64_t>, bool> _collisionStates;

	std::vector<std::weak_ptr<Object>> _objects;
	std::vector<std::weak_ptr<Object>> _objectsPool[Object::BITSET_SIZE];
};