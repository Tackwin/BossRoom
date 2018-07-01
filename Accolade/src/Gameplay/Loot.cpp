#include "Loot.hpp"

#include <functional>

#include "Player/Player.hpp"

#include "./../Managers/TimerManager.hpp"
#include "./../Managers/AssetsManager.hpp"

Loot::Loot(float r) :
	Zone(r) 
{
	collider->onEnter = std::bind(&Loot::onEnter, this, std::placeholders::_1);
	collisionMask.set((size_t)Object::BIT_TAGS::PLAYER);
}
Loot::~Loot() noexcept {
	if (_lootImpossibleKey) {
		TM::removeFunction(_lootImpossibleKey);
	}
}


void Loot::setLootType(LOOT_TYPE type) {
	cleanSprites();
	_lootType = type;
}
Loot::LOOT_TYPE Loot::getLootType() const {
	return _lootType;
}

void Loot::setWeapon(const std::string& weapon) {
	_weapon = weapon;
	cleanSprites();
	addSprite(
		"icon", sf::Sprite{ AM::getTexture(Wearable::GetWearableinfo(weapon).uiTexture) }
	);
}
const std::string& Loot::getWeapon() const {
	return _weapon;
}

void Loot::setLootable(bool lootable) {
	_lootable = lootable;
}
bool Loot::isLootable() const {
	return _lootable;
}

void Loot::render(sf::RenderTarget& target) {
	switch (_lootType) {
	case LOOT_TYPE::WEAPON:
		Zone::render(target);
		break;
	}
}

void Loot::onEnter(Object* obj) { // we know typeof(obj) is necessarly Player*
	if (!_lootable) return;

	switch (_lootType) {
	case LOOT_TYPE::WEAPON :
		Player* player = static_cast<Player*>(obj);
	
		auto player_weapon = player->getWeapon();
		player->swapWeapon(_weapon);

		setWeapon(player_weapon);
		_lootable = false;

		_lootImpossibleKey = TimerManager::addFunction(_lootImpossibleTime, 
			[&](double) -> bool {
				_lootable = true;
				_lootImpossibleKey.nullify();
				return true;
			}
		);

		break;
	}
}
