#pragma once
#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include "Const.hpp"

#include "Physics/World.hpp"

#include "Gameplay/Zone.hpp"

class Boss;
class Probe;
class Weapon;
class Player;
class Particle; 
class Projectile;
class LevelScreen;
class Level  {
public:
	Level() {};
	Level(std::shared_ptr<Boss> boss, uint32 _n);
	~Level();

	void start(LevelScreen* screen_);
	void stop();

	void update(double dt);
	void updateEnding(double dt);
	void updateRunning(double dt);
	void render(sf::RenderTarget& target);

	void startAim();
	void stopAim();

	void createLoots();

	static std::vector<std::shared_ptr<Level>> levels;
	static void createLevels();
	static void destroyLevels();

	float getNormalizedBossLife() const;

	Vector2f getPlayerPos() const;

private: // TODO: Make this private
	void removeNeeded();

	World _world;

	uint32 _n = 0u;

	std::shared_ptr<Boss> _boss = nullptr;
	std::shared_ptr<Player> _player = nullptr;

	std::vector<std::shared_ptr<Weapon>> _loot = {};

	LevelScreen* _screen = nullptr; //How the fuck i'm supposed to make LevelScreen pass this as a shared_ptr
									//(std::enable_shared_from_this has his probleme) anyway, i DON'T have the ownership

	std::vector<Zone> _zones = {};
	Disk _entranceToNext;
	
	sf::Sprite _aimSprite;
	uint8 _aimSpriteOpacity = 0u;

	std::shared_ptr<Object>  _floor = nullptr;
	std::vector<std::shared_ptr<Projectile>> _projectiles = {};
	
	bool _ended = false;
	bool _aiming = false;
	bool _running = false;

	std::shared_ptr<Zone> _zone = nullptr;
};

