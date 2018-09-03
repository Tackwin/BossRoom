#include "MeleeGuy.hpp"

#include "Managers/AssetsManager.hpp"
#include "Managers/TimerManager.hpp"

#include "Ruins/Section.hpp"

#define LOAD(x, y) if (auto i = json.find(#x); i != json.end()) info.x = y(*i);
#define SAVE(x, y) json[#x] = y(info.x);

MeleeGuyInfo MeleeGuyInfo::loadJson(nlohmann::json json) noexcept {
	MeleeGuyInfo info;

	LOAD(attackOrigin, Vector2f::loadJson);
	LOAD(startPos, Vector2f::loadJson);
	LOAD(origin, Vector2f::loadJson);
	LOAD(size, Vector2f::loadJson);

	LOAD(damage, );
	LOAD(viewRange, );
	LOAD(speed, );
	LOAD(cd, );
	LOAD(health, );
	LOAD(attackPersistence, );
	LOAD(attackRadius, );
	LOAD(attackRange, );

	return info;
}

nlohmann::json MeleeGuyInfo::saveJson(MeleeGuyInfo info) noexcept {
	nlohmann::json json;

	SAVE(attackOrigin, Vector2f::saveJson);
	SAVE(startPos, Vector2f::saveJson);
	SAVE(origin, Vector2f::saveJson);
	SAVE(size, Vector2f::saveJson);

	SAVE(damage, );
	SAVE(viewRange, );
	SAVE(speed, );
	SAVE(cd, );
	SAVE(health, );
	SAVE(attackPersistence, );
	SAVE(attackRadius, );
	SAVE(attackRange, );

	return json;
}

MeleeGuy::MeleeGuy(MeleeGuyInfo info) noexcept : info(info) {
	pos = info.startPos;

	health = info.health;
	maxHealth = info.health;

	sprite.setTexture(AM::getTexture(MeleeGuyInfo::JSON_ID));
	sprite.setOrigin(
		sprite.getTextureRect().width * info.origin.x,
		sprite.getTextureRect().height * info.origin.y
	);

	auto box = std::make_unique<Box>();
	box->setSize(info.size);
	box->dtPos.x = -info.size.x * info.origin.x;
	box->dtPos.y = -info.size.y * info.origin.y;
	collider = std::unique_ptr<Collider>(box.release());

	idMask.set(Object::ENNEMY);
	collisionMask.set(Object::STRUCTURE);

	collider->onEnter	= std::bind(&MeleeGuy::onEnter, this, std::placeholders::_1);
	collider->onExit	= std::bind(&MeleeGuy::onEnter, this, std::placeholders::_1);
}

void MeleeGuy::attachTo(NavigationPointInfo point) noexcept {
	this->currentPoint = point;
}

void MeleeGuy::enterSection(Section* section) noexcept {
	this->section = section;
}
void MeleeGuy::leaveSection() noexcept {
	section = nullptr;
}

void MeleeGuy::update(double dt) noexcept {
	auto playerPos = section->getPlayerPos();

	flatForces.push_back({ 0, G });
	cdTime -= (float)dt;

	if (!Vector2f::equal(playerPos, pos, info.viewRange)) return;

	if (Vector2f::equal(playerPos, pos, info.attackRange)) {
		if (cdTime <= 0.f) attack();
		return;
	}

	// if the player isn't in our direction
	if (
		std::signbit(pos.x - playerPos.x) != std::signbit(pos.x - currentPoint.pos.x)
	) {
		// we need to steer
		currentPoint = section->getNextNavigationPointFrom(currentPoint.id, playerPos);
	}

	walkToward();
}

void MeleeGuy::render(sf::RenderTarget& target) noexcept {
	sf::CircleShape mark{ 0.05f };
	mark.setOrigin(mark.getRadius(), mark.getRadius());
	mark.setPosition(pos);

	sprite.setPosition(pos);
	sprite.setScale(
		info.size.x / sprite.getTextureRect().width,
		info.size.y / sprite.getTextureRect().height
	);

	target.draw(sprite);
	target.draw(mark);
}

void MeleeGuy::hit(float dmg) noexcept {
	health -= dmg;
	if (health <= 0.f) {
		remove();
	}
}

bool MeleeGuy::toRemove() const noexcept {
	return removed;
}
void MeleeGuy::remove() noexcept {
	removed = true;
}

void MeleeGuy::onEnter(Object*) noexcept {

}
void MeleeGuy::onExit(Object*) noexcept {
	
}

void MeleeGuy::walkToward() noexcept {
	if (!currentPoint.id) return;

	if (Vector2f::equal(currentPoint.pos, pos, currentPoint.range)) {
		auto next = section->getNextNavigationPointFrom(
			currentPoint.id, section->getPlayerPos()
		);

		if (next.id != currentPoint.id) currentPoint = next;
		return;
	}

	if (currentPoint.pos.x < pos.x) {
		flatVelocities.push_back({ -info.speed, 0 });
	}
	else {
		flatVelocities.push_back({ +info.speed, 0 });
	}
}

void MeleeGuy::attack() noexcept {

	auto playerPos = section->getPlayerPos();

	auto attackZone = std::make_shared<Zone>(info.attackRadius);
	attackZone->collisionMask.set(Object::PLAYER);

	attackZone->pos = pos;
	attackZone->pos.x += info.size.x * info.attackOrigin.x;
	attackZone->pos.y += info.size.y * info.attackOrigin.y;
	attackZone->pos.x += (std::signbit(playerPos.x - pos.x) ? -1 : 1)
		* (info.size.x + 0.5f * info.attackRadius);

	attackZone->collisionMask.set(Object::PLAYER);

	// We know it's a player.
	attackZone->collider->onEnter =
		[dmg = info.damage, ptr = std::weak_ptr{ attackZone }]
		(Object* obj) {
			assert(dynamic_cast<Player*>(obj) != nullptr);
			assert(!ptr.expired());

			auto player = (Player*)obj;
			player->hit(dmg);
		};

	zones.push_back(attackZone);

	TM::addFunction(info.attackPersistence,
		[wp = std::weak_ptr{attackZone}](auto) -> bool {
			if (wp.expired()) return true;
			auto p = wp.lock();

			p->remove();

			return true;
		}
	);

	cdTime = info.cd;
	attackTime = info.attackPersistence;
}

const std::vector<std::shared_ptr<Zone>>& MeleeGuy::getZonesToApply() const noexcept {
	return zones;
}
void MeleeGuy::clearZones() noexcept {
	zones.clear();
}