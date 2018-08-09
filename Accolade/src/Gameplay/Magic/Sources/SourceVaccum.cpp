#include "SourceVaccum.hpp"

#include "Gameplay/Projectile.hpp"
#include "Gameplay/Magic/Spells/Spell.hpp"

#include "Utils/json_algorithms.hpp"

SourceVaccumInfo SourceVaccumInfo::loadJson(nlohmann::json json) noexcept {
	SourceVaccumInfo info;
	info.source = SourceInfo::loadJson(json);
	if (auto it = json.find("range"); it != json.end()) {
		info.range = json.at("range");
	}
	return info;
}

nlohmann::json SourceVaccumInfo::saveJson(SourceVaccumInfo info) noexcept {
	nlohmann::json json = nlohmann::json::object();

	json = merge(json, SourceInfo::saveJson(info.source));
	json["range"] = info.range;

	return json;
}

SourceVaccum::SourceVaccum(SourceVaccumInfo info) noexcept :
	Source(info.source), info_(info) 
{
	auto ptrCollider = std::make_unique<Disk>();
	ptrCollider->r = info_.range;
	ptrCollider->sensor = true;
	ptrCollider->onEnter = std::bind(&SourceVaccum::onEnter, this, std::placeholders::_1);
	ptrCollider->setPos(pos);

	idMask.set(Object::SOURCE);
	collisionMask.set(Object::MAGIC);

	collider = std::move(ptrCollider);
}

void SourceVaccum::render(sf::RenderTarget& target) noexcept {
	Source::render(target);
}

void SourceVaccum::update(double dt) noexcept {
	Source::update(dt);
}


void SourceVaccum::onEnter(Object* object) noexcept {
	if (object->idMask[Object::SPELL]) {
		dynamic_cast<Removable*>(object)->remove();
	}
}