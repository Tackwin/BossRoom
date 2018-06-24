#pragma once
#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include "./../Common.hpp"

#include "./../Physics/World.hpp"

#include "./../Gameplay/Zone.hpp"
#include "./../Gameplay/Weapon.hpp"
#include "./../Gameplay/Loot.hpp"

class Zone;
class Boss;
class Probe;
class Player;
class Particle; 
class Projectile;
class LevelScreen;
class Level  {
public:
	Level() {};
	Level(std::shared_ptr<Boss> boss, u32 _n);
	~Level();

	void start(LevelScreen* screen_);
	void stop();

	void update(double dt);
	void updateEnding(double dt);
	void updateRunning(double dt);
	void render(sf::RenderTarget& target);

	void startAim();
	void stopAim();

	void addProjectile(const std::shared_ptr<Projectile>& p);
	void delProjectile(const std::shared_ptr<Projectile>& p);
	void delProjectile(u32 i);
	void addZone(const std::shared_ptr<Zone>& z);
	void delZone(const std::shared_ptr<Zone>& z);
	void delZone(u32 i);

	void createLoots();

	static std::vector<std::shared_ptr<Level>> levels;
	static void createLevels();
	static void destroyLevels();

	float getNormalizedBossLife() const;

	Vector2f getPlayerPos() const;
	Vector2f getDirToPlayer(Vector2f pos) const noexcept;

	bool lost() const;

private:
	void removeNeeded();

	void pullPlayerObjects();

	void playerOnEnter(Object* object);
	void playerOnExit(Object* object);

	void projectileOnEnter(std::weak_ptr<Projectile> ptrProjectile, Object* object);
	void projectileOnExit(std::weak_ptr<Projectile> ptrProjectile, Object* object);

	World _world;

	u32 _n = 0u;

	std::vector<std::shared_ptr<Loot>> _loots;

	std::shared_ptr<Boss> _boss = nullptr;
	std::shared_ptr<Player> _player = nullptr;

	Box _floorHitBox;

	LevelScreen* _screen = nullptr; //How the fuck i'm supposed to make LevelScreen pass this as a shared_ptr
									//(std::enable_shared_from_this has his probleme) anyway, i DON'T have the ownership

	std::vector<std::shared_ptr<Zone>> _zones = {};
	Disk _entranceToNext;
	
	sf::Sprite _aimSprite;
	u08 _aimSpriteOpacity = 0u;

	std::shared_ptr<Object>  _floor = nullptr;
	std::vector<std::shared_ptr<Projectile>> _projectiles = {};
	std::vector<std::shared_ptr<Zone>> _playerZones = {};
	
	bool _ended = false;
	bool _aiming = false;
	bool _running = false;
};

