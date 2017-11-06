#pragma once
#include <memory>

#include <SFML/Graphics.hpp>

#include "Const.hpp"

#include "Zone.hpp"
#include "Math/Vector.hpp"

class Weapon;
class Loot : protected Zone {
public:

	enum class LOOT_TYPE : u08 {
		NONE = 0u,
		WEAPON
	};

	Loot(float r = 0.f);

	void setLootType(LOOT_TYPE type);
	LOOT_TYPE getLootType() const;

	void setWeapon(std::shared_ptr<Weapon> weapon);
	std::shared_ptr<Weapon> getWeapon() const;

	void setLootable(bool lootable = true);
	bool isLootable() const;

	void render(sf::RenderTarget& target);


private:
	void onEnter(Object* obj);

	LOOT_TYPE _lootType = LOOT_TYPE::NONE;

	bool _lootable = false;
	float _lootImpossibleTime = 2.f;

	sf::Sprite _lootSprite;
	std::shared_ptr<Weapon> _weapon;
};