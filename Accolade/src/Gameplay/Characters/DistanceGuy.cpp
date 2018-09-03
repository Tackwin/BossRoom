#include "DistanceGuy.hpp"

#include "Managers/AssetsManager.hpp"

#include "Gameplay/Projectile.hpp"
#include "Gameplay/Player/Player.hpp"
#include "Gameplay/Magic/Spells/SpellDirection.hpp"

#include "Ruins/Section.hpp"

#define X(x, y) if (auto i = json.find(#x); i != json.end()) info.x = y(*i);

DistanceGuyInfo DistanceGuyInfo::loadJson(nlohmann::json json) noexcept {
	DistanceGuyInfo info;

	X(startPos, Vector2f::loadJson);
	X(origin, Vector2f::loadJson);
	X(size, Vector2f::loadJson);
	X(health, );
	X(range, );
	X(reloadTime, );
	X(maxHealth, );
	X(contactDamage, );
	X(lifeTimeBoost, );
	X(spellDamage, );
	X(sprite, [](auto x) {return x.get<std::string>(); });

	return info;
}

nlohmann::json DistanceGuyInfo::saveJson(DistanceGuyInfo info) noexcept {
	nlohmann::json json;
	json["startPos"] = Vector2f::saveJson(info.startPos);
	json["origin"] = Vector2f::saveJson(info.origin);
	json["size"] = Vector2f::saveJson(info.size);
	json["health"] = info.health;
	json["lifeTimeBoost"] = info.lifeTimeBoost;
	json["range"] = info.range;
	json["maxHealth"] = info.maxHealth;
	json["reloadTime"] = info.reloadTime;
	json["sprite"] = info.sprite;
	json["contactDamage"] = info.contactDamage;
	json["spellDamage"] = info.contactDamage;
	return json;
}

#undef X

DistanceGuy::DistanceGuy(DistanceGuyInfo info) noexcept :
	info_(info),
	reloadTimer_(info.reloadTime)	// don't use members in this shit, the order
									// of initialization is determined by the order
									// in the header.
{
	pos = info_.startPos;
	sprite_.setTexture(AM::getTexture(info_.sprite));
	sprite_.setOrigin(
		sprite_.getTextureRect().width  * info_.origin.x,
		sprite_.getTextureRect().height * info_.origin.y
	);

	auto box = std::make_unique<Box>();
	box->setSize(info_.size);
	box->dtPos.x = -info_.size.x * info_.origin.x;
	box->dtPos.y = -info_.size.y * info_.origin.y;
	collider = std::unique_ptr<Collider>((Collider*)box.release());
	idMask.set(Object::ENNEMY);
	collisionMask.set(Object::PROJECTILE);
	collisionMask.set(Object::STRUCTURE);
	collisionMask.set(Object::PLAYER);

	collider->onEnter = std::bind(&DistanceGuy::onEnter, this, std::placeholders::_1);
	collider->onExit = std::bind(&DistanceGuy::onExit, this, std::placeholders::_1);
}

void DistanceGuy::enterSection(Section* section) noexcept {
	section_ = section;
}

void DistanceGuy::update(double dt) noexcept {

	reloadTimer_ -= (float)dt;
	auto playerPos = section_->getPlayerPos();

	if (reloadTimer_ < .3f) {
		colorToAttack_ = {
			1.f - (1/.3f) * (.3f - reloadTimer_),
			1.f - (1/.3f) * (.3f - reloadTimer_),
			1.f, 
			1.f
		};
	}

	if ((playerPos - pos).length2() < info_.range * info_.range && reloadTimer_ < 0.f) {
		SpellDirectionInfo info =
			SpellDirectionInfo::loadJson(AM::getJson(SpellDirectionInfo::JSON_ID));
		info.damage = info_.spellDamage;

		info.lifeTime *= info_.lifeTimeBoost;
		auto ptr = std::make_shared<SpellDirection>(
			section_, section_->getObject(uuid), info
		);
		auto targetMask = std::bitset<Object::SIZE>();
		targetMask.set(Object::PLAYER);
		auto dir = (section_->getPlayerPos() - pos).normalize();

		ptr->launch(dir, targetMask);
		ptr->setPos(support((float)dir.angleX(), 0.f));
		section_->addSpell(ptr);

		reloadTimer_ = info_.reloadTime;
		colorToAttack_ = { 1.f, 1.f, 1.f, 1.f };
	}


	flatForces.push_back({ 0.f, G });
}
void DistanceGuy::render(sf::RenderTarget& target) noexcept {
	sf::CircleShape mark{ 0.05f };
	mark.setOrigin(mark.getRadius(), mark.getRadius());
	mark.setPosition(pos);

	sprite_.setColor(colorToAttack_);
	sprite_.setPosition(pos);
	sprite_.setScale(
		info_.size.x / sprite_.getTextureRect().width,
		info_.size.y / sprite_.getTextureRect().height
	);
	
	target.draw(mark);
	target.draw(sprite_);
};

void DistanceGuy::onEnter(Object* object) noexcept {
	if (auto player = (Player*)object; object->idMask[Object::PLAYER]) {
		player->knockBack(
			2 * ((player->getPos() - pos).normalize() * 5 + Vector2f{ 0.f, -10.f }),
			0.5f
		);
		player->hit(info_.contactDamage);
	}
	if (auto proj = (Projectile*)object;  object->idMask[Object::PROJECTILE]) {
		info_.health -= proj->getDamage();
		if (info_.health < 0) {
			remove();
		}
		proj->remove();
	}
}

void DistanceGuy::onExit(Object*) noexcept {

}

void DistanceGuy::hit(float damage) noexcept {
	info_.health -= damage;
	if (info_.health < 0.f) {
		remove();
	}
}

Vector2f DistanceGuy::support(float a, float d) const noexcept {
	return pos + Vector2f::createUnitVector(a) * (info_.size.x / 2.f + d);
}

void DistanceGuy::remove() noexcept {
	_remove = true;
}

bool DistanceGuy::toRemove() const noexcept {
	return _remove;
}