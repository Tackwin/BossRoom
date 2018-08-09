#include "SpellTarget.hpp"

#include <cmath>
#include <random>
#include <iostream>

#include "Ruins/Section.hpp"

#include "Managers/AssetsManager.hpp"

#include "Gameplay/Characters/Slime.hpp"
#include "Gameplay/Characters/DistanceGuy.hpp"

SpellTargetInfo SpellTargetInfo::loadJson(nlohmann::json json) noexcept {
	SpellTargetInfo info;

	if (auto it = json.find("speed"); it != json.end()) {
		info.speed = *it;
	}
	if (auto it = json.find("range"); it != json.end()) {
		info.range = *it;
	}
	if (auto it = json.find("damage"); it != json.end()) {
		info.damage = *it;
	}
	if (auto it = json.find("radius"); it != json.end()) {
		info.radius = *it;
	}
	if (auto it = json.find("rotateSpeed"); it != json.end()) {
		info.rotateSpeed = *it;
	}
	if (auto it = json.find("particleGenerator"); it != json.end()) {
		info.particleGenerator = it->get<decltype(info.particleGenerator)>();
	}

	return info;
}
nlohmann::json SpellTargetInfo::saveJson(SpellTargetInfo info) noexcept {
	nlohmann::json json = nlohmann::json::object();

	json["speed"] = info.speed;
	json["range"] = info.range;
	json["radius"] = info.speed;
	json["damage"] = info.damage;
	json["rotateSpeed"] = info.rotateSpeed;
	json["particleGenerator"] = info.particleGenerator;

	return json;
}

SpellTarget::SpellTarget(
	Section* section, std::weak_ptr<Object> sender, SpellTargetInfo info
) noexcept :
	Spell(section), info_(info), sender_(sender)
{

	auto disk = std::make_unique<Disk>();
	disk->sensor = true;
	disk->r = info_.radius;
	pos = section->getPlayer()->support(0.f, 0.2f);

	id_ = uuid;

	idMask.set(Object::SPELL);
	idMask.set(Object::MAGIC);

	collider = std::move(disk);
	collider->onEnter = std::bind(&SpellTarget::onEnter, this, std::placeholders::_1);
	collider->onExit  = std::bind(&SpellTarget::onExit , this, std::placeholders::_1);
	collider->setPos(pos);

	particleGenerator_ = ParticleGenerator(
		AM::getJson(info_.particleGenerator), pos
	);

	angleToSender_ = unitaryRng(RD) * PIf * 2;
}

void SpellTarget::update(double dt) noexcept {

	if (launched_ && target_.expired()) {
		remove();
		return;
	}

	auto target = target_.lock();

	angleToSender_ += (float)dt * info_.rotateSpeed;
	if (launched_) {
		auto wanted = target->pos;

		auto vel = (wanted - pos).normalize() * info_.speed;
		flatVelocities.push_back(vel);

		particleGenerator_.setPos(pos);
		particleGenerator_.update(dt);
	}
	else {
		auto wanted =
			section_->getPlayer()->support(angleToSender_, 0.f);

		auto vel = (wanted - pos);
		vel = vel * info_.speed;
		//if (velocity.length2() > 5.f) {
		//	velocity = velocity.normalize() * 5.f;
		//}
		flatVelocities.push_back(vel);
		// flatForces.push_back(nor.applyCW(fabs).applyCW(sqrtf));
	}
}
void SpellTarget::render(sf::RenderTarget& target) noexcept {
	sf::CircleShape point{ info_.radius };
	point.setOrigin(point.getRadius(), point.getRadius());
	point.setPosition(pos);

	if (launched_) {
		point.setFillColor(Vector4f{ 0.f,1.f,0.f,1.f });
		particleGenerator_.render(target);
	}
	else {
		point.setFillColor(Vector4f{ 1.f,0.f,0.f,1.f });
	}

	target.draw(point);
}

void SpellTarget::onEnter(Object* obj) noexcept {
	if (!launched_) return;
	if (target_.expired()) return remove();

	if (obj->uuid == target_.lock()->uuid) {
		if (obj->idMask[Object::SLIME]) {
			auto ptr = (Slime*)obj;

			ptr->hit(info_.damage);
			remove();
		}
		else if (obj->idMask[Object::DISTANCE]) {
			auto ptr = (DistanceGuy*)obj;

			ptr->hit(info_.damage);
			remove();
		}
		else if (obj->idMask[Object::PLAYER]) {
			auto ptr = (Player*)obj;

			ptr->hit(info_.damage);
			remove();
		}
	}

}
void SpellTarget::onExit(Object*) noexcept {}

void SpellTarget::launch(std::weak_ptr<Object> obj) noexcept {
	target_ = obj;

	assert(!obj.expired());

	collisionMask |= obj.lock()->idMask;
	maskChanged = true;
	launched_ = true;
}

SpellTargetInfo SpellTarget::getSpellInfo() const noexcept {
	return info_;
}

void SpellTarget::remove() noexcept {
	if (launched_) Spell::remove();
}