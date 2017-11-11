#include "Gameplay/Projectile.hpp"

#include "Common.hpp"

#include "Managers/AssetsManager.hpp"
#include "Managers/TimerManager.hpp"

Projectile::Projectile() : 
	Object()
{
}
Projectile::Projectile(nlohmann::json json, 
	Vector2f pos, Vector2f dir, bool player) :

	Object(),
	_dir(dir),
	_player(player),
	_json(json),
	_update([](Projectile&, double) {}) 
{
	_speed = getJsonValue<float>(json, "speed");
	_damage = getJsonValue<float>(json, "damage");
	_radius = getJsonValue<float>(json, "radius");
	_lifespan = getJsonValue<float>(json, "lifespan");
	_destroyOthers = json["destroyOthers"];
	_dir.normalize();
	
	this->pos = pos;

	collider = &_disk;
	_disk.userPtr = this;
	_disk.r = _radius;

	idMask.set((size_t)BIT_TAGS::PROJECTILE);
	collisionMask.set((size_t)(player ? BIT_TAGS::BOSS : BIT_TAGS::PLAYER));
	collisionMask.set((size_t)BIT_TAGS::FLOOR);

	velocity = _dir * _speed;

	std::uniform_int_distribution<i32> dist(0, 2);

	_sprite = sf::Sprite(AssetsManager::getTexture(json["sprite"]));
	const auto& rect = _sprite.getTextureRect();
	_sprite.setOrigin(Vector2f((float)rect.width, (float)rect.height) / 2.f); 
	_sprite.setScale(
		3.f * _radius / rect.width,
		3.f * _radius / rect.height
	);
	_sprite.setRotation((float)(180.0 * _dir.angleX() / PId));
	_sprite.setPosition(pos);

	_key = TimerManager::addFunction(
		_lifespan, 
		"destroy", 
		[&](double) mutable ->bool {
			//_remove = true;
			return true;
		}
	);
}
Projectile::Projectile(nlohmann::json json, Vector2f pos, Vector2f dir, 
	bool player, std::function<void(Projectile&, double)> update) :

	Projectile(json, pos, dir, player)
{
	_update = update;
}/*

Projectile::Projectile(const Projectile& other) :
	Projectile(
		other._json,
		other.getPos(),
		other.getDir(),
		other.isFromPlayer(),
		other._update
	)
{}

Projectile::Projectile(const Projectile&& other) :
	Projectile(
		other._json,
		other.getPos(),
		other.getDir(),
		other.isFromPlayer(),
		other._update
	)
{}
*/

Projectile::~Projectile() {
	if (TimerManager::functionsExist(_key))
		TimerManager::removeFunction(_key);
}

void Projectile::update(double dt) {
	_update(*this, dt);
	_lifespan -= (float)dt;
	if (_lifespan < 0.f) {
		_remove = true;
	}
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
float Projectile::getDamage() const {
	return _damage;
}
bool Projectile::isFromPlayer() const {
	return _player;
}
