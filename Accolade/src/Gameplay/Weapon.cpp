#include "Weapon.hpp"

#include "./../Game.hpp"
#include "./../Common.hpp"

#include "./../Managers/AssetsManager.hpp"
#include "./../Managers/InputsManager.hpp"
#include "./../Managers/TimerManager.hpp"

#include "./../Physics/Collider.hpp"

#include "Projectile.hpp"
#include "Player/Player.hpp"
#include "Level.hpp"

Weapon::Weapon() {}

Weapon::Weapon(nlohmann::json json)
	: _json(json),
	_radius(json["radius"]),
	_name(""),
	_cost(0)
{
	_uiSprite = sf::Sprite(AssetsManager::getTexture(_json["sprite"]));
	_uiSprite.setOrigin(
		_uiSprite.getTextureRect().width * 0.5f, 
		_uiSprite.getTextureRect().height * 0.5f
	);
	setUiSpriteSize({ 100.f, 100.f });
	_uiSprite.setPosition((float)WIDTH, (float)HEIGHT);
}

Weapon::Weapon(const Weapon& other) :
	_json(other._json),
	_flags(other._flags),
	_keys(other._keys),

	_radius(other._radius),
	_equip(other._equip),
	_unEquip(other._unEquip),
	_active(other._active),
	_passive(other._passive),
	_update(other._update),
	
	_activeSounds(other._activeSounds),

	_uiSprite(other._uiSprite)
{
}

void Weapon::render(sf::RenderTarget&) {
}
void Weapon::renderGui(sf::RenderTarget& target) {
	target.draw(_uiSprite);
}

void Weapon::equip(Player& player) {
	_equip(*this, player);
}
void Weapon::unEquip(Player& player) {
	_unEquip(*this, player);
}

void Weapon::active(Player& player, u32 id_) {
	_active(*this, player, id_);
}
void Weapon::passive(Player& player, u32 id_) {
	_passive(*this, player, id_);
}

void Weapon::update(Player& player, float dt) {
	_update(*this, player, dt);
}

const sf::Sprite& Weapon::getUiSprite() const {
	return _uiSprite;
}
void Weapon::setUiSpriteSize(const Vector2f& size) {
	_uiSprite.setScale(
		size.x / _uiSprite.getTextureRect().width,
		size.y / _uiSprite.getTextureRect().height
	);
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

const nlohmann::json& Weapon::getJson() const {
	return _json;
}

void Weapon::swap(Weapon& other) {
	std::swap(_name, other._name);
	std::swap(_cost, other._cost);
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

const UUID& Weapon::getUUID() const noexcept {
	return _uuid;
}