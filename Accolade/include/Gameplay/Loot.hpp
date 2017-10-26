#pragma once
#include <memory>

#include "Zone.hpp"
#include "Math/Vector.hpp"

class Weapon;
class Loot : public Zone {
public:

	Loot(float r = 0.f);

	void setWeapon(const std::shared_ptr<Weapon>& weapon);
	std::shared_ptr<Weapon> getWeapon() const;

	void setLootable(bool lootable = true);
	bool isLootable() const;

private:
	void onEnter();

	bool _lootable = false;

	sf::Sprite _lootSprite;
	std::shared_ptr<Weapon> _weapon;
};