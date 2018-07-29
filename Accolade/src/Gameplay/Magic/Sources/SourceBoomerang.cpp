#include "SourceBoomerang.hpp"

#include "Managers/AssetsManager.hpp"

#include "Utils/json_algorithms.hpp"

#include "Ruins/Section.hpp"

#include "Gameplay/Magic/Spells/SpellBoomerang.hpp"

SourceBoomerangInfo SourceBoomerangInfo::loadJson(nlohmann::json json) noexcept {
	SourceBoomerangInfo info;
	info.source = SourceInfo::loadJson(json);
	if (auto it = json.find("range"); it != json.end()) {
		info.range = json.at("range");
	}
	return info;
}

nlohmann::json SourceBoomerangInfo::saveJson(SourceBoomerangInfo info) noexcept {
	nlohmann::json json = nlohmann::json::object();

	json = merge(json, SourceInfo::saveJson(info.source));
	json["range"] = info.range;

	return json;
}

SourceBoomerang::SourceBoomerang(SourceBoomerangInfo info) noexcept :
	Source(info.source),
	info_(info)
{
	gened = info_.source.reloadTime;
}

void SourceBoomerang::update(double dt) noexcept {
	Source::update(dt);

	gened -= (float)dt;

	auto player = section_->getPlayer();
	if (player->isBoomerangSpellAvailable()) return;

	auto playerPos = section_->getPlayerPos();
	if (gened < 0.f && Vector2f::equal(pos, playerPos, info_.range)) {
		auto spellInfo = SpellBoomerangInfo::loadJson(AM::getJson("boomerangSpell"));
		player->giveSpell(spellInfo);

		gened = info_.source.reloadTime;
	}
}
void SourceBoomerang::render(sf::RenderTarget& target) noexcept {
	Source::render(target);
}