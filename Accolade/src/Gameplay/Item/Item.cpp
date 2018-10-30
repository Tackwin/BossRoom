#include "Item.hpp"

Item::Type Item::getItemType() const noexcept {
	return Type::Generic;
}

std::string Item::getName() const noexcept {
	return name;
}
size_t Item::getCost() const noexcept {
	return cost;
}