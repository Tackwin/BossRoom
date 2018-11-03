#include "Item.hpp"

#include "AxisAlignedShooter.hpp"
#include "ImmediateRangedSword.hpp"


// Add Item,
// first the class and it's info struct.

#define LIST(x)\
	x(AxisAlignedShooter, AxisAlignedShooterInfo)\
	x(ImmediateRangedSword, ImmediateRangedSwordInfo)

Item::Type Item::getItemType() const noexcept {
	return Type::Generic;
}

std::string Item::getName() const noexcept {
	return name;
}
size_t Item::getCost() const noexcept {
	return cost;
}

ItemInfo* ItemInfo::clone() const noexcept {
#define X(val, x)\
	if (auto ptr = dynamic_cast<const x*>(this); ptr) {\
		return new x(*ptr);\
	}

	LIST(X);
#undef X
	std::abort();
}

Item* make_item(const ValuePtr<ItemInfo>& item) noexcept {
#define X(x, y)\
	if (auto ptr = dynamic_cast<y*>(item.get()); ptr) {\
		return new x(*ptr);\
	}

	LIST(X);
#undef X

	std::abort();
}

void from_json(const nlohmann::json& json, ValuePtr<ItemInfo>& item) noexcept {
	assert(json.count("type"));
	// Add Item
#define X(obj, x)\
	if (json.at("type") == x::JSON_ID) {\
		x info = json; item = std::move(ValuePtr<x>(new x(info)));return;\
	}

	LIST(X);
#undef X
	std::abort(); // Must be unreachable
}

void to_json(nlohmann::json& json, const ValuePtr<ItemInfo>& item) noexcept {

#define X(obj, x)\
	if (auto ptr = dynamic_cast<x*>(item.get()); ptr) {\
		json = *ptr; json["type"] = x::JSON_ID; return;\
	}

	LIST(X);
#undef X
	std::abort(); // Must be unreachable
}

void Item::remove() noexcept {
	to_remove = true;
}

bool Item::toRemove() const noexcept {
	return to_remove;
}