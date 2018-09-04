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
	pos				=	info.startPos;
	health			=	info.maxHealth;
	timeToDive		=	info.timeToDive();
	angleToPlayer	=	unitaryRng(RD) * 2 * PIf;

	aSprite = AnimatedSprite{ "fly" };
	aSprite.pushAnim("idle");
	aSprite.getSprite().setOrigin(
		aSprite.getSprite().getTextureRect().width / 2.f,
		aSprite.getSprite().getTextureRect().height / 2.f
	);

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
		info.radius / aSprite.getSprite().getTextureRect().width * (flipX ? -1 : 1),
		info.radius / aSprite.getSprite().getTextureRect().height
	);

	aSprite.render(target);
	target.draw(mark);

	auto newPos = Vector2f::createUnitVector(angleToPlayer) * info.minDistance
		+ section->getPlayerPos();
	mark.setPosition(newPos);
	mark.setFillColor(sf::Color::Red);
	target.draw(mark);

	if (posToDiveTo) {
		mark.setPosition(*posToDiveTo);
		mark.setFillColor(sf::Color::Green);
		target.draw(mark);
	}
#if 0
	auto sum = Vector2f{ 0, 0 };
	if (divingVel) sum += *divingVel;
	if (rotatingVel) sum += *rotatingVel;

	if (divingVel)
		Vector2f::renderLine(target, pos, pos + *divingVel, { 1.0, 0, 0, 1.0 });

	if (rotatingVel)
		Vector2f::renderLine(target, pos, pos + *rotatingVel, { 0.0, 1.0, 1.0, 1.0 });
	Vector2f::renderLine(target, pos, pos + sum, { 1.0, 0.5, 1.0, 1.0 });
#endif
}

void Fly::update(double dt) noexcept {
	auto player = section->getPlayer();

	auto perimeter = info.minDistance * 2 * C::PIf;
	auto arcCircle = info.speed;
	auto angleToRotate = (arcCircle / perimeter) * 2 * C::PIf;

	angleToPlayer += (float)dt * angleToRotate;
	angleToPlayer = std::fmodf(angleToPlayer, 2 * PIf);

	divingVel.reset();
	rotatingVel.reset();

	if (diving) {
		updateDiving(dt);
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

	auto drag = std::pow(0.2, dt);

	auto vec = [&](auto x) {
		if (x) {
			*x *= drag;
			flatVelocities.push_back(*x);

			if (x->length2() < 0.01 * 0.01) x.reset();
		}
	};
	vec(divingVel);
	vec(rotatingVel);

	auto sum = Vector2f{ 0, 0 };
	if (divingVel) sum += *divingVel;
	if (rotatingVel) sum += *rotatingVel;

	// we add just the little bit of inertia.
	velocity += sum * dt;
	velocity *= drag;

	static float timeToFlipX = 0.f;
	if (timeToFlipX <= 0.f) {
		flipX = pos.x < player->getPos().x;
		timeToFlipX = timerToFlipX;
	}
	else timeToFlipX -= (float)dt;

}

void Fly::updateDiving(double) noexcept {
	auto player = section->getPlayer();

	auto wanted = *posToDiveTo - pos;
	wanted = wanted.normalize() * info.divingSpeed;

	divingVel = wanted;

	if (Vector2f::equal(*posToDiveTo, pos, info.radius)) {
		diving = false;
		timeToDive = info.timeToDive();
		timeToDecide = 0.f;
		posToDiveTo.reset();
	}
}

void Fly::updateDeciding(double dt) noexcept {
	timeToDecide -= (float)dt;
}

void Fly::updatePreparingToDive(double dt) noexcept {
	static bool chasingLock = true;

	auto player = section->getPlayer();

	auto target =
		Vector2f::createUnitVector(angleToPlayer) * info.minDistance + player->getPos();

	if (Vector2f::equal(target, pos, info.radius / 2.f))
		chasingLock = false;

	if (!Vector2f::equal(target, pos, info.radius * 2.f))
		chasingLock = true;

	if (chasingLock)
		rotatingVel = (target - pos).normalize() * info.speed * 2;
	else
		rotatingVel = Vector2f::createUnitVector(unitaryRng(RD) * 2 * PIf) * info.speed / 4;

	timeToDive -= (float)dt;
	if (timeToDive < 0.f) {
		timeToDecide = info.timeToDecide;
		posToDiveTo = player->getPos() +
			Vector2f::createUnitVector(angleToPlayer + PIf) * info.minDistance;
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