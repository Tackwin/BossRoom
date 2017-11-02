#include "Gameplay/Projectile.hpp"

#include "Const.hpp"

#include "Managers/AssetsManager.hpp"
#include "Managers/TimerManager.hpp"

Projectile::Projectile() : 
	Object()
{
}
Projectile::Projectile(nlohmann::json json, Vector2f pos, Vector2f dir, bool player) :
	Object(),
	_dir(dir),
	_player(player),
	_json(json),
	_update([](Projectile&, double) {}) {

	_speed = getJsonValue<float>(json, "speed");
	_damage = getJsonValue<int>(json, "damage");
	_radius = getJsonValue<float>(json, "radius");
	_lifespan = getJsonValue<float>(json, "lifespan");
	_destroyOthers = json["destroyOthers"];
	_dir.normalize();
	
	this->pos = pos;

	collider = &_disk;
	_disk.userPtr = this;
	_disk.r = _radius;

	idMask |= BIT_TAGS::PROJECTILE;
	collisionMask |= (player ? BIT_TAGS::BOSS : BIT_TAGS::PLAYER);
	collisionMask |= BIT_TAGS::FLOOR;

	velocity = _dir * _speed;

	std::uniform_int_distribution<i32> dist(0, 2);

	_sprite = sf::Sprite(AssetsManager::getTexture(json["sprite"]));
	const i32 whole = _sprite.getTextureRect().width / 3;
	_sprite.setTextureRect({
		dist(RD) * whole	, dist(RD) * whole, 
		whole				,	whole
	});
	_sprite.setOrigin(_radius, _radius); // i don't know why it's off, maybe because i do shit with texture rect just above...
	_sprite.setPosition(pos);

	_key = TimerManager::addFunction(_lifespan, "destroy", [&](double)->bool {
		_remove = true;
		return true;
	});
}
Projectile::Projectile(nlohmann::json json, Vector2f pos, Vector2f dir, bool player, 
					   std::function<void(Projectile&, double)> update):
	Projectile(json, pos, dir, player) {
	_update = update;
}

Projectile::~Projectile() {
	if (TimerManager::functionsExist(_key))
		TimerManager::removeFunction(_key);
}

void Projectile::update(double dt) {
	_update(*this, dt);
}

void Projectile::render(sf::RenderTarget &target) {
	_sprite.setPosition(pos);
	target.draw(_sprite);
}

void Projectile::changeLifespan(float) {
}

bool Projectile::toRemove() {
	return _remove;
}

Vector2f Projectile::getPos() const {
	return pos;
}

void Projectile::setPos(const Vector2f& p) {
	pos = p;
}

Vector2f Projectile::getDir() const {
	return _dir;
}
void Projectile::setDir(const Vector2f& d) {
	_dir = d;
	velocity = _dir * _speed;
}

bool Projectile::toRemove() const {
	return _remove;
}
void Projectile::remove() {
	_remove = true;
}
i32 Projectile::getDamage() const {
	return _damage;
}
bool Projectile::isFromPlayer() const {
	return _player;
}
