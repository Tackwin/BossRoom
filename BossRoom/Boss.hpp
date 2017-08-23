#pragma once
#include <memory>
#include <functional>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "json.hpp"

#include "Rectangle.hpp"
#include "Vector2.hpp"

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

public: //TODO: make this private
	nlohmann::basic_json<> _json;

	int _life;
	int _maxLife;
	Vector2 _pos;
	float _radius;
	sf::Color _color;

	Level* _level;

	std::vector<Rectangle> _rectBox;
	std::vector<std::string> _keyPatterns; // (key, running)

	std::function<void(Boss&)> _init;
	std::function<void(float, Boss&)> _update;
	std::function<void(Boss&)> _unInit;

	sf::Sprite _sprite;
public:

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
};