#include "Slime.hpp"
#include "Ruins/Section.hpp"

#include "Managers/AssetsManager.hpp"

SlimeInfo SlimeInfo::loadFromJson(nlohmann::json json) noexcept {
	SlimeInfo info;

	if (auto it = json.find("sprite"); it != json.end()) {
		info.sprite = it->get<std::string>();
	}
	if (auto it = json.find("speed"); it != json.end()) {
		info.speed = it->get<float>();
	}
	if (auto it = json.find("startPos"); it != json.end()) {
		info.startPos = Vector2f::load(*it);
	}
	return info;
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
	target.draw(_sprite);
}

void Slime::onEnter(Object*) noexcept {}

void Slime::onExit(Object*) noexcept {}