#include "Gameplay/Projectile.hpp"

#include "Global/Const.hpp"
#include "Managers/AssetsManager.hpp"
#include "Managers/TimerManager.hpp"

Projectile::Projectile() {
}
Projectile::Projectile(nlohmann::json json, Vector2 pos, Vector2 dir, bool player) :
	_dir(dir),
	_player(player),
	_json(json),
	_update([](Projectile&, float) {}) {


	_speed = getJsonValue<float>(json, "speed");
	_damage = getJsonValue<int>(json, "damage");
	_radius = getJsonValue<float>(json, "radius");
	_lifespan = getJsonValue<float>(json, "lifespan");
	_destroyOthers = json["destroyOthers"];
	_dir.normalize();
	
	this->pos = pos;
	_disk.r = _radius;

	std::uniform_int_distribution<int32> dist(0, 2);

	_sprite = sf::Sprite(AssetsManager::getTexture(json["sprite"]));
	const int32 whole = _sprite.getTextureRect().width / 3;
	_sprite.setTextureRect({
		dist(RNG) * whole	, dist(RNG) * whole, 
		whole				,	whole
	});
	_sprite.setOrigin(_radius, _radius);
	_sprite.setPosition(pos);

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
	pos += _dir * _speed *dt;
	_update(*this, dt);
}

void Projectile::render(sf::RenderTarget &target) {
	_sprite.setPosition(pos);
	target.draw(_sprite);
	_disk.render(target);
}

void Projectile::changeLifespan(float) {
}

bool Projectile::toRemove() {
	return _remove;
}

Vector2 Projectile::getPos() const {
	return pos;
}

void Projectile::setPos(const Vector2& p) {
	pos = p;
}

Vector2 Projectile::getDir() const {
	return _dir;
}
void Projectile::setDir(const Vector2& d) {
	_dir = d;
	velocity = _dir * _speed;
}

bool Projectile::toRemove() const {
	return _remove;
}
void Projectile::remove() {
	_remove = true;
}
int32_t Projectile::getDamage() const {
	return _damage;
}
