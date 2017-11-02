#include "Gameplay/Weapon.hpp"

#include "Game.hpp"
#include "Const.hpp"

#include "Managers/AssetsManager.hpp"
#include "Managers/InputsManager.hpp"
#include "Managers/TimerManager.hpp"

#include "Physics/Collider.hpp"

#include "Gameplay/Projectile.hpp"
#include "Gameplay/Player.hpp"
#include "Gameplay/Level.hpp"

Weapon::Weapon() {}

Weapon::Weapon(std::shared_ptr<Player> player, nlohmann::json json)
	: _player(player),
	_json(json),
	_radius(json["radius"]),
	_name(""),
	_cost(0)
{
	_uiSprite = sf::Sprite(AssetsManager::getTexture(_json["sprite"]));
	_uiSprite.setScale(2, 2);
	_uiSprite.setOrigin(
		(float)_uiSprite.getTextureRect().width, 
		(float)_uiSprite.getTextureRect().height
	);
	_uiSprite.setPosition((float)WIDTH, (float)HEIGHT);
}

Weapon::Weapon(const Weapon& other) :
	_player(other._player),
	_json(other._json),
	_flags(other._flags),
	_keys(other._keys),

	_radius(other._json["radius"]),
	_equip(other._equip),
	_unEquip(other._unEquip),
	_active(other._active),
	_passive(other._passive),
	_update(other._update),
	
	_activeSounds(other._activeSounds)
{
	const std::string& str = _json["sprite"];

	_uiSprite = sf::Sprite(AssetsManager::getTexture(str));
	_uiSprite.setScale(2, 2);
	_uiSprite.setOrigin(
		_uiSprite.getTextureRect().width * 0.5f,
		_uiSprite.getTextureRect().height * 0.5f
	);
	_uiSprite.setPosition((float)WIDTH, (float)HEIGHT);
}

void Weapon::render(sf::RenderTarget&) {
}
void Weapon::renderGui(sf::RenderTarget& target) {
	target.draw(_uiSprite);
}

void Weapon::equip() {
	_equip(*this);
}
void Weapon::unEquip() {
	_unEquip(*this);
}

void Weapon::active(u32 id_) {
	_active(*this, id_);
}
void Weapon::passive(u32 id_) {
	_passive(*this, id_);
}

void Weapon::update(float dt) {
	_update(*this, dt);
}

const sf::Sprite& Weapon::getUiSprite() const {
	return _uiSprite;
}
void Weapon::setUiSpritePos(const Vector2f& pos_) {
	_uiSprite.setPosition(pos_);
}


void Weapon::addProjectile(const std::shared_ptr<Projectile>& projectile) {
	_projectileBuffer.push_back(projectile);
}
const std::vector<std::shared_ptr<Projectile>>& Weapon::getProjectileBuffer() const {
	return _projectileBuffer;
}
void Weapon::clearProjectileBuffer() {
	_projectileBuffer.clear();
}

std::string Weapon::getStringSoundActive(u32 n) const {
	return _json["sounds"]["active"][n].get<std::string>();
}

void Weapon::setName(const std::string& name) {
	_name = name;
}
std::string Weapon::getName() const {
	return _name;
}

void Weapon::setCost(const u32& cost) {
	_cost = cost;
}
u32 Weapon::getCost() const {
	return _cost;
}

void Weapon::swap(Weapon& other) {
	std::swap(_name, other._name);
	std::swap(_cost, other._cost);
	std::swap(_player, other._player);
	std::swap(_uiSprite, other._uiSprite);
	std::swap(_radius, other._radius);
	std::swap(_keys, other._keys);
	std::swap(_flags, other._flags);
	std::swap(_json, other._json);
	std::swap(_activeSounds, other._activeSounds);
	std::swap(_projectileBuffer, other._projectileBuffer);
	std::swap(_equip, other._equip);
	std::swap(_unEquip, other._unEquip);
	std::swap(_active, other._active);
	std::swap(_passive, other._passive);
	std::swap(_update, other._update);
}

Weapon& Weapon::operator=(const Weapon& other) {
	Weapon tmp(other);
	swap(tmp);
	return *this;
}
