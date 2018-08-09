#include "SourceTarget.hpp"

#include "Managers/AssetsManager.hpp"

#include "Utils/json_algorithms.hpp"

#include "Ruins/Section.hpp"

#include "Gameplay/Magic/Spells/SpellTarget.hpp"
#include "Gameplay/Player/Player.hpp"

SourceTargetInfo SourceTargetInfo::loadJson(nlohmann::json json) noexcept {
	SourceTargetInfo info;
	info.source = SourceInfo::loadJson(json);
	if (auto it = json.find("range"); it != json.end()) {
		info.range = json.at("range");
	}
	return info;
}

nlohmann::json SourceTargetInfo::saveJson(SourceTargetInfo info) noexcept {
	nlohmann::json json = nlohmann::json::object();

	json = merge(json, SourceInfo::saveJson(info.source));
	json["range"] = info.range;

	return json;
}

SourceTarget::SourceTarget(SourceTargetInfo info) noexcept :
	Source(info.source),
	info_(info)
{
	gened = info_.source.reloadTime;

	auto ptrCollider = std::make_unique<Disk>();
	ptrCollider->r = info_.range;
	ptrCollider->sensor = true;
	ptrCollider->onEnter = std::bind(&SourceTarget::onEnter, this, std::placeholders::_1);
	ptrCollider->onExit  = std::bind(&SourceTarget::onExit,  this, std::placeholders::_1);
	
	idMask.set(Object::SOURCE);
	collisionMask.set(Object::PLAYER);

	collider = std::move(ptrCollider);
}

void SourceTarget::update(double dt) noexcept {
	Source::update(dt);

	gened -= (float)dt;



	if (player_ && gened < 0.f) {
		if (player_->isBoomerangSpellAvailable()) return;
		
		auto spellInfo =
			SpellTargetInfo::loadJson(AM::getJson(SpellTargetInfo::JSON_ID));
		player_->giveSpell(spellInfo);

		gened = info_.source.reloadTime;
	}
}

void SourceTarget::render(sf::RenderTarget& target) noexcept {
	Source::render(target);

	sf::CircleShape range{ info_.range };
	range.setOrigin(range.getRadius(), range.getRadius());
	range.setPosition(pos);

	range.setOutlineThickness(0.01f);
	range.setFillColor(Vector4f{ 0.5f, 0.0f, 0.f, 0.2f });
	range.setOutlineColor(Vector4f{ 0.5f, 0.0f, 0.f, 1.f });
	target.draw(range);
}

void SourceTarget::onEnter(Object* object) noexcept {
	if (auto player = (Player*)object; object->idMask[Object::PLAYER]) {
		player_ = player;
	}
}

void SourceTarget::onExit(Object* object) noexcept {
	if (object->idMask[Object::PLAYER]) {
		player_ = nullptr;
	}
}