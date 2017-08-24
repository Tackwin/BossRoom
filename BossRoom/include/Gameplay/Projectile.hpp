#pragma once
#include <functional>

#include <SFML/Graphics.hpp>

#include "3rd/json.hpp"

#include "Math/Vector2.hpp"
#include "Global/Const.hpp"

class Projectile {
public:
	Projectile();
	Projectile(nlohmann::json json, Vector2 pos, Vector2 dir, bool player);
	Projectile(nlohmann::json json, Vector2 pos, Vector2 dir, bool player, 
			   std::function<void(Projectile&, float)> update);
	~Projectile();

	std::function<void(Projectile&, float)> _update;

public:
	nlohmann::json _json;

	std::string _key;

	Vector2 _pos;
	Vector2 _dir;

	float _lifespan;
	float _speed;
	float _radius;
	int _damage;

	bool _player;
	bool _remove = false;
	bool _damageable = true;
	bool _destroyOthers = false;

	sf::Sprite _sprite;

	void update(float dt);
	void render(sf::RenderTarget& target);

	void changeLifespan(float lifeSpan);
};

