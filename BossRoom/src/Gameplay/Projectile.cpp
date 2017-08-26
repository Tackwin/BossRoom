#include "Gameplay/Projectile.hpp"

#include "Global/Const.hpp"
#include "Managers/AssetsManager.hpp"
#include "Managers/TimerManager.hpp"

Projectile::Projectile() {
}
Projectile::Projectile(nlohmann::json json, Vector2 pos, Vector2 dir, bool player) :
	_pos(pos),
	_dir(dir),
	_player(player),
	_json(json),
	_update([](Projectile&, float) {}) {
	
	_dir.normalize();

	_speed = getJsonValue<float>(json, "speed");
	_damage = getJsonValue<int>(json, "damage");
	_radius = getJsonValue<float>(json, "radius");
	_lifespan = getJsonValue<float>(json, "lifespan");
	_destroyOthers = json["destroyOthers"];

	std::uniform_int_distribution<int32> dist(0, 2);

	const std::string idleSprite = json["sprites"]["idle"];
	AssetsManager::loadTexture(idleSprite, std::string(ASSETS_PATH) + idleSprite);
	_sprite = sf::Sprite(AssetsManager::getTexture(idleSprite));
	const int32 whole = _sprite.getTextureRect().width / 3;
	_sprite.setTextureRect({
		dist(RNG) * whole	, dist(RNG) * whole, 
		whole				,	whole
	});
	_sprite.setOrigin(_radius, _radius);
	_sprite.setPosition(_pos);

	_key = TimerManager::addFunction(_lifespan, "destroy", [&](float)->bool {
		_remove = true;
		return true;
	});
}
Projectile::Projectile(nlohmann::json json, Vector2 pos, Vector2 dir, bool player, 
					   std::function<void(Projectile&, float)> update):
	Projectile(json, pos, dir, player) {
	_update = update;
}

Projectile::~Projectile() {
	if (TimerManager::functionsExist(_key))
		TimerManager::removeFunction(_key);
}

void Projectile::update(float dt) {
	_pos += _dir * _speed * dt;
	_update(*this, dt);
}

void Projectile::render(sf::RenderTarget &target) {
	_sprite.setPosition(_pos);
	target.draw(_sprite);
}

void Projectile::changeLifespan(float) {
}

