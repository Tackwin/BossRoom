#pragma once
#include <memory>
#include <functional>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "./../3rd/json.hpp"

#include "./../Math/Vector.hpp"

#include "./../Graphics/AnimatedSprite.hpp"
#include "./../Graphics/ParticleGenerator.hpp"

#include "./../Physics/Object.hpp"

#include "./../Common.hpp"
#include <unordered_set>

class Level;
class Projectile;
class Boss : public Object {
public:
	Boss() {};
	//TODO
	Boss(Boss&);
	Boss(const nlohmann::basic_json<>& json,
		 std::function<void(double, Boss&)> updateFunction,
		 std::function<void(Boss&)> initFunction,
		 std::function<void(Boss&)> unInitFunction);

	~Boss() noexcept;

	void enterLevel(Level* level);
	void exitLevel();

	void die();
	
	void update(double dt);
	void render(sf::RenderTarget& target);

	void hit(float damage);
	void shoot(const std::shared_ptr<Projectile>& projectile);
	void shoot(
		const nlohmann::json& json, 
		const Vector2f& pos, 
		const Vector2f& dir
	);

	Vector2f getPos() const;

	float getRadius() const;

	float getLife() const;
	float getMaxLife() const;

	Level* getLevel() const;

	void addKeyTimer(UUID key);
	void delKeyTimer(UUID key);

	const std::vector<std::shared_ptr<Projectile>>& getProtectilesToShoot() const;
	void clearProtectilesToShoot();

	static std::vector<std::shared_ptr<Boss>> bosses;
	static void createBosses();
	static void destroyBosses();

private:
	nlohmann::basic_json<> _json;

	float _life;
	float _maxLife;
	float _radius;
	sf::Color _color;

	Level* _level;

	Disk* _disk;

	std::vector<std::shared_ptr<Projectile>> _projectilesToShoot;

	UUID _blinkDownKey{ UUID::null };

	std::vector<UUID> _keyPatterns;

	std::function<void(Boss&)> _init;
	std::function<void(double, Boss&)> _update;
	std::function<void(Boss&)> _unInit;

	AnimatedSprite _sprite;

	std::vector<sf::Sound> _sounds;

	std::vector<ParticleGenerator*> _particleEffects;
private:
	void collision(Object* obj);
};