#include "Spell.hpp"

#include "Ruins/Section.hpp"

SpellInfo SpellInfo::loadJson(nlohmann::json) noexcept {
	return {};
}

nlohmann::json SpellInfo::saveJson(SpellInfo) noexcept {
	return {};
}

Spell::Spell(Section* section) noexcept : section_(section) {}

void Spell::enter(Section* section) noexcept {
	section_ = section;
}
void Spell::leave() noexcept {
	section_ = nullptr;
}