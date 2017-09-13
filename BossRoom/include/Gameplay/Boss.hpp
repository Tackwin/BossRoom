#pragma once
#include <memory>
#include <functional>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "3rd/json.hpp"

#include "Graphics/AnimatedSprite.hpp"
#include <Physics/Collider.hpp>
#include "Math/Rectangle.hpp"
#include "Math/Vector2.hpp"

class Level;
class Projectile;
class Boss {
public:
	Boss() {};
	Boss(const nlohmann::basic_json<>& json,
		 std::function<void(float, Boss&)> updateFunction,
		 std::function<void(Boss&)> initFunction,
		 std::function<void(Boss&)> unInitFunction);
	~Boss();

	void enterLevel(Level* level);
	void exitLevel();

	void die(); //POTATO NOOOOOOOOOOOOOOO :(
	
	void update(float dt);
	void render(sf::RenderTarget& target);

	void hit(unsigned int damage);
	void addProjectile(const std::shared_ptr<Projectile>& projectile);

	Vector2 getDirToFire();

	static std::vector<std::shared_ptr<Boss>> bosses;
	static void createBosses();
	static void destroyBosses();

public: //TODO: make this private
	nlohmann::basic_json<> _json;

	int _life;
	int _maxLife;
	Vector2 _pos;
	float _radius;
	sf::Color _color;

	Disk _disk;

	Level* _level;

	std::vector<std::string> _keyPatterns; // (key, running)

	std::function<void(Boss&)> _init;
	std::function<void(float, Boss&)> _update;
	std::function<void(Boss&)> _unInit;

	AnimatedSprite _sprite;

	std::vector<sf::Sound> _sounds;
};