#include "Spell.hpp"

#include "Ruins/Section.hpp"

Spell::Spell(Section* section) noexcept : section_(section) {}

void Spell::enter(Section* section) noexcept {
	section_ = section;
}
void Spell::leave() noexcept {
	section_ = nullptr;
}

UUID Spell::getUuid() const noexcept {
	return id_;
}

void Spell::remove() noexcept {
	_remove = true;
}

bool Spell::toRemove() const noexcept {
	return _remove;
}
