#include "MeleeGuy.hpp"

#define LOAD(x, y) if (auto i = json.find(#x); i != json.end()) info.x = y(*i);
#define SAVE(x, y) json[#x] = y(info.x);

MeleeGuyInfo MeleeGuyInfo::loadJson(nlohmann::json json) noexcept {
	MeleeGuyInfo info;

	LOAD(damage, );
	LOAD(viewRange, );
	LOAD(speed, );
	LOAD(cd, );
	LOAD(health, );

	return info;
}

nlohmann::json MeleeGuyInfo::saveJson(MeleeGuyInfo info) noexcept {
	nlohmann::json json;

	SAVE(damage, );
	SAVE(viewRange, );
	SAVE(speed, );
	SAVE(cd, );
	SAVE(health, );

	return json;
}

MeleeGuy::MeleeGuy(MeleeGuyInfo info) noexcept : info(info) {
	health = info.health;
	maxHealth = info.health;
}

void MeleeGuy::enterSection(Section* section) noexcept {
	this->section = section;
}
void MeleeGuy::leaveSection() noexcept {
	section = nullptr;
}

void MeleeGuy::update(double) noexcept {

}

void MeleeGuy::render(sf::RenderTarget&) noexcept {

}

void MeleeGuy::hit(float dmg) noexcept {
	health -= dmg;
	if (health < 0.f) {
		remove();
	}
}

bool MeleeGuy::toRemove() const noexcept {
	return removed;
}
void MeleeGuy::remove() noexcept {
	removed = true;
}