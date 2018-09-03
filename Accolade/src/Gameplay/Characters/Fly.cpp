#include "Fly.hpp"
#include "Common.hpp"

#include "Ruins/Section.hpp"


#define SAVE(x, y) json[#x] = y(info.x);

nlohmann::json FlyInfo::saveJson(FlyInfo info) noexcept {
	nlohmann::json json;

	SAVE(startPos, Vector2f::saveJson);

	SAVE(maxHealth, );
	SAVE(minDistance, );
	SAVE(timeToDive, );
	SAVE(damage, );
	SAVE(radius, );
	SAVE(speed, );
	SAVE(divingSpeed, );
	SAVE(timeToDecide, );

	return json;
}

#undef SAVE
#define LOAD(x, y) if (auto i = json.find(#x); i != json.end()) info.x = y(*i);

FlyInfo FlyInfo::loadJson(nlohmann::json json) noexcept {
	FlyInfo info;

	LOAD(startPos, Vector2f::loadJson);

	LOAD(maxHealth, );
	LOAD(minDistance, );
	LOAD(timeToDive, );
	LOAD(damage, );
	LOAD(radius, );
	LOAD(speed, );
	LOAD(divingSpeed, );
	LOAD(timeToDecide, );

	return info;
}

#undef LOAD

Fly::Fly(FlyInfo info) noexcept : info(info) {
	pos			=	info.startPos;
	health		=	info.maxHealth;
	timeToDive	=	info.timeToDive();

	aSprite = AnimatedSprite{ "fly" };
	aSprite.pushAnim("idle");

	auto disk = std::make_unique<Disk>();
	disk->r = info.radius;
	disk->sensor = true;

	collider = std::unique_ptr<Collider>(disk.release());
	collisionMask.set(Object::PLAYER);
	idMask.set(Object::ENNEMY);

	collider->onEnter	= std::bind(&Fly::onEnter, this, std::placeholders::_1);
	collider->onExit	= std::bind(&Fly::onExit , this, std::placeholders::_1);
}

void Fly::enterSection(Section* section) noexcept {
	this->section = section;
}
void Fly::leaveSection() noexcept {
	this->section = nullptr;
}

void Fly::render(sf::RenderTarget& target) noexcept {
	sf::CircleShape mark{ 0.05f };
	mark.setOrigin(mark.getRadius(), mark.getRadius());
	mark.setPosition(pos);

	aSprite.getSprite().setPosition(pos);
	aSprite.getSprite().setScale(
		info.radius / aSprite.getSprite().getTextureRect().width,
		info.radius / aSprite.getSprite().getTextureRect().height
	);

	aSprite.render(target);
	target.draw(mark);

	auto newPos = Vector2f::createUnitVector(angleToPlayer) * info.minDistance
		+ section->getPlayerPos();
	mark.setPosition(newPos);
	mark.setFillColor(sf::Color::Red);
	target.draw(mark);

	if (!isnan(angleToDiveTo)) {
		newPos = Vector2f::createUnitVector(angleToDiveTo) * info.minDistance
			+ section->getPlayerPos();
		mark.setPosition(newPos);
		mark.setFillColor(sf::Color::Green);
		target.draw(mark);
	}
}

void Fly::update(double dt) noexcept {
	auto player = section->getPlayer();

	auto dist = player->getPos() - pos;

	auto perimeter = info.minDistance * 2 * C::PIf;
	auto arcCircle = info.speed;
	auto angleToRotate = (arcCircle / perimeter) * 2 * C::PIf;

	angleToPlayer += (float)dt * angleToRotate;
	angleToPlayer = std::fmodf(angleToPlayer, 2 * PIf);

	if (diving) {
		updateDiving(dt);
	}
	else if (dist.length2() > info.minDistance * info.minDistance) {
		auto l = dist.length();
		velocity += dist.normalize() * std::min(l, info.speed);
	}
	else if (timeToDive > 0.f) {
		updatePreparingToDive(dt);
	}
	else if (timeToDecide > 0.f) {
		updateDeciding(dt);
	}
	else {
		diving = true;
	}

	velocity *= std::pow(0.2, dt);
}

void Fly::updateDiving(double) noexcept {
	auto player = section->getPlayer();

	auto target = Vector2f::createUnitVector(angleToDiveTo) *
		info.minDistance + player->getPos();
	auto wanted = target - pos;
	wanted *= wanted.length();

	if (wanted.length2() > info.speed * info.speed) {
		wanted = wanted.normalize() * info.divingSpeed;
	}

	velocity += wanted;

	if (Vector2f::equal(target, pos, info.radius)) {
		diving = false;
		timeToDive = info.timeToDive();
		timeToDecide = 0.f;
		angleToDiveTo = NAN;
	}
}

void Fly::updateDeciding(double dt) noexcept {
	timeToDecide -= (float)dt;

	velocity += Vector2f::createUnitVector(PIf + angleToDiveTo) * info.speed / 20.f;
}

void Fly::updatePreparingToDive(double dt) noexcept {
	auto player = section->getPlayer();

	auto target =
		Vector2f::createUnitVector(angleToPlayer) * info.minDistance + player->getPos();

	velocity += (target - pos) * std::min((target - pos).length(), info.speed);

	timeToDive -= (float)dt;
	if (timeToDive < 0.f) {
		timeToDecide = info.timeToDecide;
		angleToDiveTo = std::fmodf(angleToPlayer + PIf, PIf * 2);
	}
}

bool Fly::toRemove() const noexcept {
	return removed;
}
void Fly::remove() noexcept {
	removed = true;
}

void Fly::hit(float damage) noexcept {
	health -= damage;

	if (health <= 0) remove();
}

void Fly::onEnter(Object* object) noexcept {
	if (auto hitee = dynamic_cast<Hitable*>(object); hitee) hitee->hit(info.damage);
	if (auto bumpee = dynamic_cast<Bumpable*>(object); bumpee)
		bumpee->bump(Vector2f::createUnitVector(unitaryRng(RD) * 2 * PIf));
}
void Fly::onExit(Object*) noexcept {}