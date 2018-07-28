#include "SpellBoomerang.hpp"

#include "Ruins/Section.hpp"

#include <random>

SpellBoomerangInfo SpellBoomerangInfo::loadJson(nlohmann::json json) noexcept {
	SpellBoomerangInfo info;

	if (auto it = json.find("speed"); it != json.end()) {
		info.speed = *it;
	}

	return info;
}
nlohmann::json SpellBoomerangInfo::saveJson(SpellBoomerangInfo info) noexcept {
	nlohmann::json json = nlohmann::json::object();

	json["speed"] = info.speed;

	return json;
}


SpellBoomerang::SpellBoomerang(
	Section* section, std::shared_ptr<Object> target, SpellBoomerangInfo info
) noexcept : Spell(section), target_(target), info_(info) {
	pos_ = section->getPlayerPos();
}

void SpellBoomerang::update(double dt) noexcept {
	pos_ += (target_->pos - pos_).normalize() * info_.speed * dt;
}
void SpellBoomerang::render(sf::RenderTarget& target) noexcept {
	sf::CircleShape mark{ 0.05f };
	mark.setOrigin(mark.getRadius(), mark.getRadius());
	mark.setPosition(pos_);

	target.draw(mark);
}