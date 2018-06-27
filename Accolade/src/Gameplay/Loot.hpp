#pragma once
#include <memory>

#include <SFML/Graphics.hpp>

#include "./../Common.hpp"

#include "./../Math/Vector.hpp"

#include "Zone.hpp"
#include "Weapon.hpp"

class Loot : public Zone {
public:

	enum class LOOT_TYPE : u08 {
		NONE = 0u,
		WEAPON
	};

	Loot(float r);
	~Loot() noexcept;

	Loot(Loot&) = delete;
	Loot& operator=(Loot&) = delete;

	Loot(Loot&&) = default;
	Loot& operator=(Loot&&) = default;

	void setLootType(LOOT_TYPE type);
	LOOT_TYPE getLootType() const;

	void setWeapon(UUID weapon);
	UUID getWeapon() const;

	void setLootable(bool lootable = true);
	bool isLootable() const;

	void render(sf::RenderTarget& target);


private:
	void onEnter(Object* obj);

	LOOT_TYPE _lootType = LOOT_TYPE::NONE;

	bool _lootable = false;
	float _lootImpossibleTime = 2.f;

	sf::Sprite _lootSprite;
	UUID _weapon;

	UUID _lootImpossibleKey{ UUID::null };
};