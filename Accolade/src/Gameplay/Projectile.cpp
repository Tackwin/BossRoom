#include "Projectile.hpp"

#include "./../Common.hpp"

#include "./../Managers/AssetsManager.hpp"
#include "./../Managers/TimerManager.hpp"

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
	static int i = 0;

	i++;

	if (i == 7) {
		printf("debug");
	}
	_speed = getJsonValue<float>(json, "speed");
	_damage = getJsonValue<float>(json, "damage");
	_radius = getJsonValue<float>(json, "radius");
	_lifespan = getJsonValue<float>(json, "lifespan");
	_destroyOthers = json["destroyOthers"];
	_dir.normalize();
	
	this->pos = pos;

	collider = std::make_unique<Disk>();
	_disk = (Disk*)collider.get();
	_disk->userPtr = this;
	_disk->r = _radius;

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
}
Projectile::Projectile(nlohmann::json json, Vector2f pos, Vector2f dir, 
	bool player, std::function<void(Projectile&, double)> update) :
	Projectile(json, pos, dir, player)
{
	_update = update;
}

void Projectile::update(double dt) {
	_update(*this, dt);
	_lifespan -= (float)dt;
	if (_lifespan < 0.f) {
		_remove = true;
	}
}

void Projectile::render(sf::RenderTarget &target) {
	sf::CircleShape mark{ 0.05f };
	mark.setOrigin(mark.getRadius(), mark.getRadius());
	mark.setPosition(getPos());

	_sprite.setPosition(pos);
	target.draw(_sprite);
	target.draw(mark);
}

void Projectile::changeLifespan(float) {
}

void Projectile::explode(bool v) {
	_exploded = v;
}
bool Projectile::toExplode() const {
	return _exploded;
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

const nlohmann::json& Projectile::getJson() const {
	return _json;
}