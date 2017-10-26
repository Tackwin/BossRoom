#include "Gameplay/Loot.hpp"

#include <functional>

Loot::Loot(float r) :
	Zone(r) 
{
	collider->onEnter = std::bind(&Loot::onEnter, this);

	collisionMask |= Object::BIT_TAGS::PLAYER;
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

void Loot::onEnter() {
	if (!_lootable) return;


}
