#pragma once

#include <SFML/Graphics.hpp>

#include "3rd/json.hpp"

#include "Components/Removable.hpp"
#include "Components/Clonable.hpp"
#include "Memory/ValuePtr.hpp"

struct ItemInfo {
	virtual ItemInfo* clone() const noexcept;
};

class Item : public Removable, public Clonable {
public:
	enum class Type : size_t {
		Main_Weapon = 0,
		Conso,
		Generic,
		Count
	};

	virtual ~Item() noexcept = default;

	virtual void mount() noexcept = 0;
	virtual void unMount() noexcept = 0;

	virtual void update(double dt) noexcept = 0;
	virtual void render(sf::RenderTarget& target) noexcept = 0;

	virtual void remove() noexcept;
	virtual bool toRemove() const noexcept;

	virtual ValuePtr<ItemInfo> getInfo() const noexcept = 0;

	virtual sf::Texture& getIconTexture() const noexcept = 0;
	virtual std::string getName() const noexcept;
	virtual size_t getCost() const noexcept;

	virtual Type getItemType() const noexcept;
protected:

	std::string name;
	size_t cost{ 0 };

	bool to_remove{ false };
};

extern Item* make_item(const ValuePtr<ItemInfo>& item) noexcept;
extern void from_json(const nlohmann::json& json, ValuePtr<ItemInfo>& item) noexcept;
extern void to_json(nlohmann::json& json, const ValuePtr<ItemInfo>& item) noexcept;