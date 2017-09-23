#pragma once
#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include <Global/Const.hpp>
#include <Gameplay/Zone.hpp>
#include <Physics/World.hpp>

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

	void update(float dt);
	void updateEnding(float dt);
	void updateRunning(float dt);
	void render(sf::RenderTarget& target);

	void startAim();
	void stopAim();

	void createLoots();

	static std::vector<std::shared_ptr<Level>> levels;
	static void createLevels();
	static void destroyLevels();

	float getNormalizedBossLife() const;

public: // TODO: Make this private
	void removeNeeded();

	World _worldExp;

	uint32 _n;

	std::shared_ptr<Boss> _boss;
	std::shared_ptr<Player> _player;

	std::vector<std::shared_ptr<Weapon>> _loot;

	LevelScreen* _screen = nullptr; //How the fuck i'm supposed to make LevelScreen pass this as a shared_ptr
									//(std::enable_shared_from_this has his probleme) anyway, i DON'T have the ownership

	std::vector<Zone> _zones;
	Disk _entranceToNext;
	
	sf::Sprite _aimSprite;
	uint8 _aimSpriteOpacity;

	std::shared_ptr<Object>  _floor;
	std::vector<std::shared_ptr<Projectile>> _projectiles;
	
	bool _ended = false;
	bool _aiming = false;
	bool _running = false;
};

