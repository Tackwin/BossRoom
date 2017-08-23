#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "json.hpp"

#include "Vector2.hpp"
#include "Const.hpp"

class Level;
class Weapon;
class Projectile;
class Player {
public:
	Player() {};
	Player(const nlohmann::json& json);
	~Player();

public: //TODO: Make private

	int32 _AK;
	int _upK;
	int _leftK;
	int _downK;
	int _rightK;
	int32 _slowK;
	int32 _dashK;

	int _life;
	int _maxLife;
	Vector2 _pos;
	Vector2 _dir;
	float _speed;
	float _radius;
	sf::Color _color;
	float _dashRange;
	float _slowSpeed;
	float _invincibilityTime;

	std::string _projectilesKey;

	nlohmann::basic_json<> _json;

	std::string _keyCdAction;
	std::string _keyCdActionCaC;

	bool _invincible = false;
	bool _freeze = false;

	Level* _level;
	std::shared_ptr<Weapon> _weapon;

	sf::Sprite _sprite;
	sf::Sound _hitSound;

public:
	void enterLevel(Level* level);
	void exitLevel();

	void update(float dt);
	void render(sf::RenderTarget& target);

	void hit(unsigned int damage);
	void startCaC();

	Vector2 getDirToFire();

	void swapWeapon(std::shared_ptr<Weapon> weapon);
	void addProjectile(const std::shared_ptr<Projectile>& projectile);
};