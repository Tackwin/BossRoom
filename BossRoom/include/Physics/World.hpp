#pragma once

//Everything here is juste shit, i'm coming back in the old way

#include <array>
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

	void addProjectile(const std::shared_ptr<Projectile>& projectile);
private:

	void burstParticle(const std::shared_ptr<const Boss>& boss, const Vector2& pos);

	void removeNeeded();
	void pullAllProjectiles();
	void updatePlayers(float dt);
	void updateBosses(float dt);
	void updateProjectiles(float dt);
	void updateParticles(float dt);

	std::vector<std::weak_ptr<Boss>> _bosses;
	std::vector<std::shared_ptr<Zone>> _zones;
	std::vector<std::weak_ptr<Player>> _players;
	std::vector<std::shared_ptr<Particle>> _particles;
	std::vector<std::shared_ptr<Projectile>>  _projectiles;
};