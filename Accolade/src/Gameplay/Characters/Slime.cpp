#include <iostream>

#include "brx/Parser.hpp"

#include "Slime.hpp"
#include "Ruins/Section.hpp"

#include "Managers/AssetsManager.hpp"

#include "Gameplay/Projectile.hpp"
#include "Gameplay/Player/Player.hpp"

#define LOAD(x, y) if (auto i = json.find(#x); i != json.end()) info.x = y(*i);
#define SAVE(x, y) json[#x] = y(info.x);

SlimeInfo SlimeInfo::loadJson(nlohmann::json json) noexcept {
	SlimeInfo info;

	LOAD(sprite, [](auto x) {return x.get<std::string>(); });

	LOAD(startPos, Vector2f::loadJson);
	LOAD(size, Vector2f::loadJson);

	LOAD(speed, );
	LOAD(health, );
	LOAD(contactDamage, );
	LOAD(viewRange, );
	LOAD(maxHealth, );

	return info;
}

nlohmann::json SlimeInfo::saveJson(SlimeInfo info) noexcept {
	nlohmann::json json;

	SAVE(sprite, );

	SAVE(startPos, Vector2f::saveJson);
	SAVE(size, Vector2f::saveJson);

	SAVE(speed, );
	SAVE(health, );
	SAVE(contactDamage, );
	SAVE(viewRange, );
	SAVE(maxHealth, );

	return json;
}

Slime::Slime(SlimeInfo info) noexcept : _info(info), sprite(info.sprite) {
	sprite.pushAnim("walk");

	sprite.getSprite().setTexture(AM::getTexture(_info.sprite));
	AM::getTexture(_info.sprite).setSmooth(false);
	sprite.getSprite().setOrigin(
		sprite.getSprite().getTextureRect().width * 0.5f,
		sprite.getSprite().getTextureRect().height * 0.9f
	);
	pos = _info.startPos;
	auto box = std::make_unique<Box>();
	box->setSize(_info.size);
	box->dtPos.x -= _info.size.x * 0.5f;
	box->dtPos.y -= _info.size.y * 0.9f;
	collider = std::unique_ptr<Collider>((Collider*)box.release());
	idMask.set(Object::SLIME);
	idMask.set(Object::ENNEMY);
	collisionMask.set(Object::PROJECTILE);
	collisionMask.set(Object::STRUCTURE);
	collisionMask.set(Object::PLAYER);

	collider->onEnter	= std::bind(&Slime::onEnter, this, std::placeholders::_1);
	collider->onExit	= std::bind(&Slime::onExit , this, std::placeholders::_1);
}

void Slime::enterSection(Section* section) noexcept {
	_section = section;
}

void Slime::update(double) noexcept {
	auto playerPos = _section->getPlayerPos();

	flatForces.push_back({ 0, C::G });
	
	if (!Vector2f::equalf(playerPos, pos, _info.viewRange)) return;
	faceX = playerPos.x > pos.x;

	auto navPointPos = currentPoint_.pos;

	// if the player isn't in our direction
	if (
		std::signbit(pos.x - playerPos.x) != std::signbit(pos.x - navPointPos.x)
	) {
		// we need to steer
		currentPoint_ = _section->getNextNavigationPointFrom(currentPoint_.id, playerPos);
	}

	walkToward();
}

void Slime::render(sf::RenderTarget& target) noexcept {
	sprite.getSprite().setPosition(pos);
	sprite.getSprite().setScale(
		_info.size.x / sprite.getSprite().getTextureRect().width * (faceX ? 1.f : -1.f),
		_info.size.y / sprite.getSprite().getTextureRect().height
	);

	sf::CircleShape mark(0.05f);
	mark.setOrigin(mark.getRadius(), mark.getRadius());
	mark.setPosition(pos);
	sprite.render(target);
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
	else if (
		auto player = (Player*)object; 
		object->idMask[Object::PLAYER] && !player->isInvicible()
	) {
		player->knockBack(
			2 * ((player->getPos() - pos).normalize() * 5 + Vector2f{0.f, -10.f}),
			0.5f
		);
		player->hit(_info.contactDamage);
	}
}

void Slime::onExit(Object*) noexcept {}

void Slime::remove() noexcept {
	_remove = true;
}

bool Slime::toRemove() const noexcept {
	return _remove;
}

void Slime::walkToward() noexcept {
	if (!currentPoint_.id) return;

	if (Vector2f::equal(currentPoint_.pos, pos, currentPoint_.range)) {
		auto next = _section->getNextNavigationPointFrom(
			currentPoint_.id, _section->getPlayerPos()
		);

		if (next.id != currentPoint_.id) currentPoint_ = next;
		return;
	}

	if (currentPoint_.pos.x < pos.x) {
		flatVelocities.push_back({ -_info.speed, 0 });
	} else {
		flatVelocities.push_back({ +_info.speed, 0 });
	}
}

void Slime::attachTo(NavigationPointInfo point) noexcept {
	currentPoint_ = point;
}