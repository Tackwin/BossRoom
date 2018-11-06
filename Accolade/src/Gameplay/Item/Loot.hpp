#pragma once
#include <memory>

#include <SFML/Graphics.hpp>

#include "Common.hpp"

#include "Math/Vector.hpp"

#include "Entity/OwnId.hpp"

#include "Gameplay/Zone.hpp"
#include "Gameplay/Wearable/Wearable.hpp"
#include "Gameplay/Item/Item.hpp"

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

	// >DEPECRATED
	void setWeapon(const std::string& weapon);
	// >DEPECRATED
	const std::string& getWeapon() const;

	void update(double dt) noexcept;

	void setItem(OwnId<Item>&& item) noexcept;
	OwnId<Item>& getItem() noexcept;

	void setLootable(bool lootable = true);
	bool isLootable() const;

	void render(sf::RenderTarget& target);

private:
	void onEnter(Object* obj);

	LOOT_TYPE _lootType = LOOT_TYPE::NONE;

	bool _lootable{ false };
	float _lootImpossibleTime{ 2.f };

	OwnId<Item> item;

	sf::Sprite _lootSprite;
	std::string _weapon;

	UUID _lootImpossibleKey{ UUID::zero() };
};