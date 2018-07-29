#include "SpellBoomerang.hpp"

#include <cmath>
#include <random>
#include <iostream>

#include "Ruins/Section.hpp"

#include "Gameplay/Characters/Slime.hpp"

SpellBoomerangInfo SpellBoomerangInfo::loadJson(nlohmann::json json) noexcept {
	SpellBoomerangInfo info;

	if (auto it = json.find("speed"); it != json.end()) {
		info.speed = *it;
	}
	if (auto it = json.find("radius"); it != json.end()) {
		info.radius = *it;
	}
	if (auto it = json.find("damage"); it != json.end()) {
		info.damage = *it;
	}
	if (auto it = json.find("rotateSpeed"); it != json.end()) {
		info.rotateSpeed = *it;
	}

	return info;
}
nlohmann::json SpellBoomerangInfo::saveJson(SpellBoomerangInfo info) noexcept {
	nlohmann::json json = nlohmann::json::object();

	json["speed"] = info.speed;
	json["radius"] = info.speed;
	json["damage"] = info.damage;
	json["rotateSpeed"] = info.rotateSpeed;

	return json;
}

SpellBoomerang::SpellBoomerang(Section* section, SpellBoomerangInfo info) noexcept : 
	Spell(section), info_(info)
{
	pos = section->getPlayer()->support(0.f, 0.2f);

	auto disk = std::make_unique<Disk>();
	disk->sensor = true;
	disk->r = info_.radius;
	
	id_ = Object::uuid;

	idMask.set(Object::SPELL);

	collider = std::move(disk);
	collider->onEnter = std::bind(&SpellBoomerang::onEnter, this, std::placeholders::_1);
	collider->onExit  = std::bind(&SpellBoomerang::onExit , this, std::placeholders::_1);
}

SpellBoomerang::~SpellBoomerang() noexcept {
 }

void SpellBoomerang::update(double dt) noexcept {
	if (launched_ && target_.expired()) {
		remove();
		return;
	}

	auto target = target_.lock();

	angleToPlayer_ += (float)dt * info_.rotateSpeed;
	if (launched_) {
		auto wanted = target->pos;

		flatVelocities.push_back((wanted - pos).normalize() * info_.speed);
	}
	else {
		auto wanted = section_->getPlayer()->support(angleToPlayer_, 0.f);

		//if (velocity.length2() > 5.f) {
		//	velocity = velocity.normalize() * 5.f;
		//}
		flatVelocities.push_back((wanted - pos) * info_.rotateSpeed);
		// flatForces.push_back(nor.applyCW(fabs).applyCW(sqrtf));
	}
}
void SpellBoomerang::render(sf::RenderTarget& target) noexcept {
	sf::CircleShape point{ info_.radius };
	point.setOrigin(point.getRadius(), point.getRadius());
	point.setPosition(pos);

	if (launched_) {
		point.setFillColor(Vector4f{ 0.f,1.f,0.f,1.f });
	}
	else {
		point.setFillColor(Vector4f{ 1.f,0.f,0.f,1.f });
	}

	target.draw(point);
}

void SpellBoomerang::onEnter(Object* obj) noexcept {
	if (!launched_) return;
	remove();

	if (obj->idMask[Object::SLIME]) {
		auto ptr = (Slime*)obj;

		ptr->hit(info_.damage);
	}
}
void SpellBoomerang::onExit(Object*) noexcept {}

void SpellBoomerang::launch(std::weak_ptr<Object> obj) noexcept {
	target_ = obj;

	collisionMask.set(Object::SLIME);
	maskChanged = true;
	launched_ = true;
}