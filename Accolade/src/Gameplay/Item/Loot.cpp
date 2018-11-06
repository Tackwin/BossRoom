#include "Loot.hpp"

#include <functional>

#include "Gameplay/Player/Player.hpp"

#include "Managers/TimerManager.hpp"
#include "Managers/AssetsManager.hpp"

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

void Loot::setItem(OwnId<Item>&& item) noexcept {
	this->item = std::move(item);
	cleanSprites();
	addSprite("icon", sf::Sprite{es_instance->get(this->item)->getIconTexture()});
}
OwnId<Item>& Loot::getItem() noexcept {
	return item;
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

void Loot::update(double) noexcept {}

void Loot::render(sf::RenderTarget& target) {
	Zone::render(target);
}

void Loot::onEnter(Object* obj) { // we know decltype(obj) is necessarly Player*
	if (!_lootable) return;
	auto player = (Player*)obj;

	switch (_lootType) {
	case LOOT_TYPE::WEAPON :
		auto player_weapon = player->getWeapon();
		player->swapWeapon(_weapon);

		if (!player_weapon) {
			_remove = true;
			return;
		}

		setWeapon(player_weapon.value());
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
	if (item && player->canMount(item)) {
		player->mountItem(std::move(item));
		remove();
	}
}
