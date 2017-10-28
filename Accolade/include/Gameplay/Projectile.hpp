#pragma once
#include <functional>

#include <SFML/Graphics.hpp>

#include "3rd/json.hpp"

#include "Const.hpp"

#include "Math/Vector.hpp"

#include "Physics/Collider.hpp"
#include "Physics/Object.hpp"

class Projectile : public Object {
public:
	Projectile();
	Projectile(nlohmann::json json, Vector2f pos, Vector2f dir, bool player);
	Projectile(nlohmann::json json, Vector2f pos, Vector2f dir, bool player, 
			   std::function<void(Projectile&, double)> update);
	~Projectile();


	Vector2f getPos() const;
	void setPos(const Vector2f& p);

	Vector2f getDir() const;
	void setDir(const Vector2f& d);

	bool toRemove() const;
	void remove();

	i32 getDamage() const;

	bool isFromPlayer() const;

	void update(double dt);
	void render(sf::RenderTarget& target);

	void changeLifespan(float lifeSpan);

	bool toRemove();
private:
	nlohmann::json _json;

	std::string _key;

	Vector2f _dir;

	std::function<void(Projectile&, double)> _update;
	
	float _lifespan;
	float _speed;
	float _radius;
	i32 _damage;

	Disk _disk;

	bool _player;
	bool _remove = false;
	bool _damageable = true;
	bool _destroyOthers = false;

	sf::Sprite _sprite;

};

