#pragma once
#include <functional>

#include <SFML/Graphics.hpp>

#include "3rd/json.hpp"

#include "Math/Vector2.hpp"

#include "Global/Const.hpp"

#include "Physics/Collider.hpp"
#include "Physics/Object.hpp"

class Projectile : public Object {
public:
	Projectile();
	Projectile(nlohmann::json json, Vector2 pos, Vector2 dir, bool player);
	Projectile(nlohmann::json json, Vector2 pos, Vector2 dir, bool player, 
			   std::function<void(Projectile&, double)> update);
	~Projectile();


	Vector2 getPos() const;
	void setPos(const Vector2& p);

	Vector2 getDir() const;
	void setDir(const Vector2& d);

	bool toRemove() const;
	void remove();

	int32_t getDamage() const;

	bool isFromPlayer() const;
public:
	nlohmann::json _json;

	std::string _key;

	Vector2 _dir;

	std::function<void(Projectile&, double)> _update;
	
	float _lifespan;
	float _speed;
	float _radius;
	int32_t _damage;

	Disk _disk;

	bool _player;
	bool _remove = false;
	bool _damageable = true;
	bool _destroyOthers = false;

	sf::Sprite _sprite;

	void update(float dt);
	void render(sf::RenderTarget& target);

	void changeLifespan(float lifeSpan);

	bool toRemove();
};

