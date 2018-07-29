#include "Slime.hpp"
#include "Ruins/Section.hpp"

#include "Managers/AssetsManager.hpp"

#include "Gameplay/Projectile.hpp"
#include "Gameplay/Player/Player.hpp"

SlimeInfo SlimeInfo::loadJson(nlohmann::json json) noexcept {
	SlimeInfo info;

	if (auto it = json.find("sprite"); it != json.end()) {
		info.sprite = it->get<std::string>();
	}
	if (auto it = json.find("speed"); it != json.end()) {
		info.speed = *it;
	}
	if (auto it = json.find("startPos"); it != json.end()) {
		info.startPos = Vector2f::loadJson(*it);
	}
	if (auto it = json.find("health"); it != json.end()) {
		info.health = *it;
	}
	if (auto it = json.find("maxHealth"); it != json.end()) {
		info.maxHealth = *it;
	}
	return info;
}

nlohmann::json SlimeInfo::saveJson(SlimeInfo info) noexcept {
	nlohmann::json json;
	json["startPos"] = Vector2f::saveJson(info.startPos);
	json["health"] = info.health;
	json["maxHealth"] = info.maxHealth;
	json["sprite"] = info.sprite;
	json["speed"] = info.speed;

	return json;
}

Slime::Slime(SlimeInfo info) noexcept : _info(info) {
	_sprite.setTexture(AM::getTexture(_info.sprite));
	_sprite.setOrigin(
		_sprite.getTextureRect().width * 0.5f,
		_sprite.getTextureRect().height * 0.9f
	);
	pos = _info.startPos;
	auto box = std::make_unique<Box>();
	box->setSize(_info.size);
	box->dtPos.x -= _info.size.x * 0.5f;
	box->dtPos.y -= _info.size.y * 0.9f;
	collider = std::unique_ptr<Collider>((Collider*)box.release());
	idMask.set(Object::SLIME);
	collisionMask.set(Object::STRUCTURE);
	collisionMask.set(Object::PROJECTILE);
	collisionMask.set(Object::PLAYER);

	collider->onEnter	= std::bind(&Slime::onEnter, this, std::placeholders::_1);
	collider->onExit	= std::bind(&Slime::onExit , this, std::placeholders::_1);
}

void Slime::enterSection(Section* section) noexcept {
	_section = section;
}

void Slime::update(double) noexcept {
	auto playerPos = _section->getPlayerPos();

	if (pos.x < playerPos.x) {
		flatVelocities.push_back({ +_info.speed, 0 });
	}
	else {
		flatVelocities.push_back({ -_info.speed, 0 });
	}

	flatForces.push_back({ 0, C::G });
}

void Slime::render(sf::RenderTarget& target) noexcept {
	_sprite.setPosition(pos);
	_sprite.setScale(
		_info.size.x / _sprite.getTextureRect().width,
		_info.size.y / _sprite.getTextureRect().height
	);

	sf::CircleShape mark(0.05f);
	mark.setOrigin(mark.getRadius(), mark.getRadius());
	mark.setPosition(pos);
	target.draw(_sprite);
	target.draw(mark);
}

void Slime::hit(float damage) noexcept {
	_info.health -= damage;

	if (_info.health < 0.f) {
		remove();
	}
}

void Slime::onEnter(Object* object) noexcept {
	if (auto proj = (Projectile*)object;  object->idMask[Object::PROJECTILE]) {
		_info.health -= proj->getDamage();
		if (_info.health < 0) {
			remove();
		}
		proj->remove();
	}
	else if (auto player = (Player*)object; object->idMask[Object::PLAYER]) {
		player->knockBack(
			2 * ((player->getPos() - pos).normalize() * 5 + Vector2f{0.f, -10.f}),
			0.5f
		);
	}
}

void Slime::onExit(Object*) noexcept {}