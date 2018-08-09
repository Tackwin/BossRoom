#include "SourceDirection.hpp"

#include "Managers/AssetsManager.hpp"

#include "Utils/json_algorithms.hpp"

#include "Gameplay/Player/Player.hpp"

nlohmann::json SourceDirectionInfo::saveJson(SourceDirectionInfo info) noexcept {
	nlohmann::json json = nlohmann::json::object();

	json = merge(json, SourceInfo::saveJson(info.source));
	json["range"] = info.range;

	return json;
}
SourceDirectionInfo SourceDirectionInfo::loadJson(nlohmann::json json) noexcept {
	SourceDirectionInfo info;
	info.source = SourceInfo::loadJson(json);
	if (auto it = json.find("range"); it != json.end()) {
		info.range = json.at("range");
	}
	return info;
}

SourceDirection::SourceDirection(SourceDirectionInfo info) noexcept :
	Source(info.source),
	info_(info)
{
	auto ptrCollider = std::make_unique<Disk>();
	ptrCollider->r = info_.range;
	ptrCollider->sensor = true;
	ptrCollider->onEnter =
		std::bind(&SourceDirection::onEnter, this, std::placeholders::_1);
	ptrCollider->onExit =
		std::bind(&SourceDirection::onExit, this, std::placeholders::_1);
	
	idMask.set(Object::SOURCE);
	collisionMask.set(Object::PLAYER);

	collider = std::move(ptrCollider);

	timer_ = info_.source.reloadTime;
}

void SourceDirection::update(double dt) noexcept {
	Source::update(dt);

	timer_ -= (float)dt;

	if (player_ && timer_ < 0.f) {
		auto spellInfo =
			SpellDirectionInfo::loadJson(AM::getJson(SpellDirectionInfo::JSON_ID));

		player_->giveSpell(spellInfo);

		timer_ = info_.source.reloadTime;
	}
}

void SourceDirection::render(sf::RenderTarget& target) noexcept {
	Source::render(target);
}

void SourceDirection::onEnter(Object* object) noexcept {
	if (object->idMask[Object::PLAYER]) {
		player_ = (Player*)object;
	}
}
void SourceDirection::onExit(Object* object) noexcept {
	if (object->idMask[Object::PLAYER]) {
		player_ = nullptr;
	}
}