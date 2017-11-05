#include "Gameplay/Loot.hpp"

#include <functional>

#include "Gameplay/Player.hpp"


Loot::Loot(float r) :
	Zone(r) 
{
	collider->onEnter = std::bind(&Loot::onEnter, this, std::placeholders::_1);
	collisionMask.set((size_t)Object::BIT_TAGS::PLAYER);
}


void Loot::setLootType(LOOT_TYPE type) {
	_lootType = type;
}
Loot::LOOT_TYPE Loot::getLootType() const {
	return _lootType;
}

void Loot::setWeapon(const std::shared_ptr<Weapon>& weapon) {
	_weapon = weapon;
}
std::shared_ptr<Weapon> Loot::getWeapon() const {
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
		_weapon->setUiSpritePos(pos);
		_weapon->renderGui(target);
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

		_weapon = player_weapon;
		break;
	}

	setRemove(true);
}
