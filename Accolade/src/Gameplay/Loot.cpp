#include "Loot.hpp"

#include <functional>

#include "Player/Player.hpp"

#include "./../Managers/TimerManager.hpp"

Loot::Loot(float r) :
	Zone(r) 
{
	collider->onEnter = std::bind(&Loot::onEnter, this, std::placeholders::_1);
	collisionMask.set((size_t)Object::BIT_TAGS::PLAYER);
}


void Loot::setLootType(LOOT_TYPE type) {
	cleanSprites();
	_lootType = type;
}
Loot::LOOT_TYPE Loot::getLootType() const {
	return _lootType;
}

void Loot::setWeapon(UUID weapon) {
	_weapon = weapon;
	cleanSprites();
	addSprite("icon", Weapon::_weapons[weapon].getUiSprite());
}
UUID Loot::getWeapon() const {
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
	
		auto& player_weapon = player->getWeapon().getUUID();
		player->swapWeapon(_weapon);

		setWeapon(player_weapon);
		_lootable = false;

		TimerManager::addFunction(_lootImpossibleTime, 
			[&lootable = _lootable](double) -> bool {
				lootable = true;
				return true;
			}
		);

		break;
	}
}
