#pragma once
#include <functional>

#include <SFML/Graphics.hpp>

#include "./../3rd/json.hpp"

#include "./../Common.hpp"

#include "./../Math/Vector.hpp"

#include "./../Physics/Collider.hpp"
#include "./../Physics/Object.hpp"

#include "Components/Removable.hpp"

class Projectile : public Object, public Removable {
public:
	Projectile();
	Projectile(nlohmann::json json, Vector2f pos, Vector2f dir, bool player);
	Projectile(nlohmann::json json, Vector2f pos, Vector2f dir, bool player, 
			   std::function<void(Projectile&, double)> update);

	Vector2f getPos() const;
	void setPos(const Vector2f& p);

	Vector2f getDir() const;
	void setDir(const Vector2f& d);

	void explode(bool v = true);
	bool toExplode() const;

	float getDamage() const;

	bool isFromPlayer() const;

	void update(double dt);
	void render(sf::RenderTarget& target);

	void changeLifespan(float lifeSpan);

	const nlohmann::json& getJson() const;
private:
	nlohmann::json _json;

	Vector2f _dir;

	std::function<void(Projectile&, double)> _update;
	
	float _lifespan;
	float _speed;
	float _radius;
	float _damage;

	Disk* _disk;

	bool _player;
	bool _exploded = false;
	bool _damageable = true;
	bool _destroyOthers = false;

	sf::Sprite _sprite;
};

