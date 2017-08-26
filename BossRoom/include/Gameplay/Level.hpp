#pragma once
#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Global/Const.hpp"

class Boss;
class Probe;
class Weapon;
class Player;
class Particle; 
class Projectile;
class Level  {
public: // TODO: Make this private
	uint32 _n;

	std::shared_ptr<Boss> _boss;
	std::shared_ptr<Player> _player;

	std::vector<std::shared_ptr<Probe>> _probes;

	std::vector<std::shared_ptr<Particle>> _particles;

	std::vector<std::shared_ptr<Projectile>> _projectiles;
	std::vector<std::weak_ptr<Projectile>> _projectilesBoss;
	std::vector<std::weak_ptr<Projectile>> _projectilesPlayer;
	
	std::vector<std::shared_ptr<Weapon>> _loot;

	sf::CircleShape _entranceToNext;
	
	sf::Sprite _aimSprite;
	uint8 _aimSpriteOpacity;

	bool _ended = false;
	bool _aiming = false;
	bool _running = false;
public:
	Level() {};
	Level(std::shared_ptr<Boss> boss, uint32 _n);
	~Level();

	void start();
	void stop();

	void update(float dt);
	void updateEnding(float dt);
	void updateRunning(float dt);
	void updateParticles(float dt);
	void render(sf::RenderTarget& target);

	void CaCHit(unsigned int d);

	void startAim();
	void stopAim();

	void createLoots();

	static std::vector<std::shared_ptr<Level>> levels;
	static void createLevels();
	static void destroyLevels();

	void addProjectile(const std::shared_ptr<Projectile>& projectile);
};

