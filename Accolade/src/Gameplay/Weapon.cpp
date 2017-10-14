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

Weapon::Weapon(std::shared_ptr<Player> player, nlohmann::json json)
	: _player(player),
	_json(json),
	_radius(json["radius"]),
	_lootZone(std::make_shared<Zone>(_radius)){

	_lootedSprite = sf::Sprite(AssetsManager::getTexture(_json["sprite"]));
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
	_lootable(other._lootable),
	_loot(other._loot),
	_flags(other._flags),
	_keys(other._keys),

	_radius(other._json["radius"]),
	_equip(other._equip),
	_unEquip(other._unEquip),
	_active(other._active),
	_passive(other._passive),
	_update(other._update),
	
	_activeSounds(other._activeSounds),
	_lootedPos(other._lootedPos) 

{

	_lootZone = std::make_shared<Zone>(_radius);

	const std::string& str = _json["sprite"];

	_lootedSprite = sf::Sprite(AssetsManager::getTexture(str));
	_uiSprite = sf::Sprite(AssetsManager::getTexture(str));
	_uiSprite.setScale(2, 2);
	_uiSprite.setOrigin(
		(float)_uiSprite.getTextureRect().width / 2.f,
		(float)_uiSprite.getTextureRect().height / 2.f
	);
	_uiSprite.setPosition((float)WIDTH, (float)HEIGHT);
}


Weapon::~Weapon() {

}

void Weapon::render(sf::RenderTarget& target) {
	if (_loot)
		target.draw(_lootedSprite);
}
void Weapon::renderGui(sf::RenderTarget& target) {
	target.draw(_uiSprite);
}
void Weapon::loot(Vector2f pos_) {
	_loot = true;
	_looted = false;
	_lootedPos = pos_;
	_lootable = true;
	_lootedSprite.setPosition(pos_);
	_lootZone->pos = _lootedPos;
	_lootZone->setRadius(_radius);
	_lootZone->inside = [&](auto) mutable -> void { 
		if (_lootable)
			_looted = true; 
	};
}

void Weapon::equip() {
	_equip(*this);
}
void Weapon::unEquip() {
	_unEquip(*this);
}

void Weapon::active(uint32 id_) {
	_active(*this, id_);
}
void Weapon::passive(uint32 id_) {
	_passive(*this, id_);
}

void Weapon::update(float dt) {
	_update(*this, dt);
}

std::shared_ptr<Zone>& Weapon::getLootZone() {
	return _lootZone;
}

bool Weapon::isLooted() const {
	return _looted;
}

bool Weapon::isLootable() const {
	return _lootable;
}
const sf::Sprite & Weapon::getUiSprite() const {
	return _uiSprite;
}
void Weapon::setLootable(bool lootable) {
	_lootable = lootable;
}