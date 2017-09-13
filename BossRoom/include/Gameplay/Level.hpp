#pragma once
#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include <Global/Const.hpp>
#include <Gameplay/Zone.hpp>
#include <Physics/Collider.hpp>

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
	void updateParticles(float dt);
	void render(sf::RenderTarget& target);

	void bossDied();

	void startAim();
	void stopAim();

	void createLoots();

	static std::vector<std::shared_ptr<Level>> levels;
	static void createLevels();
	static void destroyLevels();

	void addProjectile(const std::shared_ptr<Projectile>& projectile);

	float getNormalizedBossLife() const;

public: // TODO: Make this private
	void updatePlayer(float dt);
	void updateBoss(float dt);

	void burstParticle(const Vector2& pos);

	uint32 _n;

	std::shared_ptr<Boss> _boss;
	std::shared_ptr<Player> _player;

	std::vector<std::shared_ptr<Probe>> _probes;

	std::vector<std::shared_ptr<Particle>> _particles;

	std::vector<std::shared_ptr<Projectile>> _projectiles;
	std::vector<std::weak_ptr<Projectile>> _projectilesBoss;
	std::vector<std::weak_ptr<Projectile>> _projectilesPlayer;
	
	std::vector<std::shared_ptr<Weapon>> _loot;

	LevelScreen* _screen = nullptr; //How the fuck i'm supposed to make LevelScreen pass this as a shared_ptr
									//(std::enable_shared_from_this has his probleme) anyway, i DON'T have the ownership

	std::vector<Zone> _zones;
	Disk _entranceToNext;
	
	sf::Sprite _aimSprite;
	uint8 _aimSpriteOpacity;

	bool _ended = false;
	bool _aiming = false;
	bool _running = false;
};

