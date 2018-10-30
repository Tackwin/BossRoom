#include "FirstBoss.hpp"

#include <algorithm>
#include <type_traits>

#include "Managers/AssetsManager.hpp"
#include "Gameplay/Projectile.hpp"
#include "Ruins/Section.hpp"
#include "Utils/meta_algorithms.hpp"

#define FROM(x) info.x = json.at(#x)

void from_json(const nlohmann::json& json, FirstBossInfo& info) noexcept {
	FROM(sniper_shots);
	FROM(random_fire_orbs);

	FROM(max_life);
	FROM(directional_fire_rate);
	FROM(directional_fire_duration);
	FROM(sniper_aim_time);
	FROM(sniper_calm_time);
	FROM(cd_directional);
	FROM(cd_random_fire);
	FROM(cd_snipe);
	FROM(random_fire_spread);
	FROM(random_fire_aim);
	FROM(random_fire_estimated_time);

	FROM(hitbox);

	FROM(start_pos);
	FROM(origin);

	FROM(texture).get<std::string>();

	FROM(hit_particle);
	FROM(directional_fire_projectile);
	FROM(sniper_projectile);
	FROM(random_fire_projectile);
}

#undef FROM
#define TO(x) json[#x] = info.x

void to_json(nlohmann::json& json, const FirstBossInfo& info) noexcept {
	TO(sniper_shots);
	TO(random_fire_orbs);

	TO(max_life);
	TO(directional_fire_rate);
	TO(directional_fire_duration);
	TO(sniper_aim_time);
	TO(sniper_calm_time);
	TO(cd_directional);
	TO(cd_random_fire);
	TO(cd_snipe);

	TO(random_fire_spread);
	TO(random_fire_aim);
	TO(random_fire_estimated_time);

	TO(hitbox);

	TO(start_pos);
	TO(origin);

	TO(texture);

	TO(hit_particle);
	TO(directional_fire_projectile);
	TO(sniper_projectile);
	TO(random_fire_projectile);
}

#undef TO

FirstBossInfo FirstBossInfo::loadJson(const nlohmann::json& json) noexcept {
	FirstBossInfo i;
	from_json(json, i);
	return i;
}

nlohmann::json FirstBossInfo::saveJson(const FirstBossInfo& info) noexcept {
	nlohmann::json j;
	to_json(j, info);
	return j;
}

FirstBoss::FirstBoss(const FirstBossInfo& info) noexcept : info(info) {
	sprite.setTexture(AM::getTexture(info.texture));
	sprite.setOrigin(
		sprite.getTextureRect().width * info.origin.x,
		sprite.getTextureRect().height * info.origin.y
	);
	sprite.setScale(
		info.hitbox.w / sprite.getTextureRect().width * (flip_x ? -1 : 1),
		info.hitbox.h / sprite.getTextureRect().height
	);

	auto box = std::make_unique<Box>();
	box->setSize(info.hitbox.size);
	box->dtPos = info.hitbox.pos;
	box->onEnter = std::bind(&FirstBoss::onEnter, this, std::placeholders::_1);
	collider = std::move(box);

	idMask.set(Object::BIT_TAGS::BOSS);
	collisionMask.set(Object::BIT_TAGS::PROJECTILE);
	collisionMask.set(Object::BIT_TAGS::STRUCTURE);

	pos = info.start_pos;
	life = info.max_life;
}

void FirstBoss::enterSection(Section* section) noexcept {
	this->section = section;

	directional_task =
		scheduler.every(info.cd_directional, std::bind(&FirstBoss::directionalFire, this));
	snipe_task = scheduler.every(info.cd_snipe, std::bind(&FirstBoss::snipe, this));
	random_fire_task =
		scheduler.every(info.cd_random_fire, std::bind(&FirstBoss::randomFire, this));

	scheduler.pause(snipe_task);
	scheduler.pause(random_fire_task);
}
void FirstBoss::leaveSection() noexcept {
	section = nullptr;
}

void FirstBoss::update(double dt) noexcept {
	scheduler.update(dt);

	for (auto& particles : particle_effects) {
		particles->update(dt);
	}
	
	flatForces.push_back({ 0, C::G });

	removeNeeded();
}
void FirstBoss::removeNeeded() noexcept {
	auto pred = [](auto x) {
		if constexpr (std::is_pointer_v<decltype(x)> || is_smart_ptr_v<decltype(x)>)
			return x->toRemove();
		else
			return x.toRemove();
	};
	
	for (size_t i = particle_effects.size() - 1; i + 1 != 0; i--) {
		if (particle_effects[i]->toRemove())
			particle_effects.erase(std::begin(particle_effects) + i);
	}
}

void FirstBoss::render(sf::RenderTarget& target) noexcept {
	sprite.setPosition(pos);
	target.draw(sprite);
}

void FirstBoss::hit(float damage) noexcept {
	static bool flag_p2_activated{ false };
	static bool flag_p3_activated{ false };

	life -= damage;
	if (!flag_p2_activated && life < 0.6f * info.max_life) {
		scheduler.resume(snipe_task);
		flag_p2_activated = true;
	}
	if (!flag_p3_activated && life < 0.3f * info.max_life) {
		scheduler.resume(random_fire_task);
		flag_p3_activated = true;
	}
	if (life < 0) {
		life = 0;
		remove();
	}
}
void FirstBoss::remove() noexcept {
	removed = true;
}
bool FirstBoss::toRemove() const noexcept {
	return removed;
}

const FirstBossInfo& FirstBoss::getInfo() const noexcept {
	return info;
}

const std::vector<std::shared_ptr<Projectile>>&
FirstBoss::getProtectilesToShoot() const noexcept {
	return projectiles_to_shoot;
}

void FirstBoss::clearProtectilesToShoot() noexcept {
	projectiles_to_shoot.clear();
}

void FirstBoss::onEnter(Object* obj) noexcept {
	if (
		auto ptr = (Projectile*)obj;
		obj->idMask.test((size_t)Object::BIT_TAGS::PROJECTILE) && ptr->isFromPlayer()
	) {
		hit(ptr->getDamage());

		auto x = std::make_unique<ParticleGenerator>(info.hit_particle, ptr->getPos());
		x->start();
		particle_effects.push_back(std::move(x));

		ptr->remove();
	}
}

Vector2f FirstBoss::support(float a, float d) const noexcept {
	return info.hitbox.support(a, d);
}

void FirstBoss::directionalFire() noexcept {
	size_t n = (size_t)(info.directional_fire_duration * info.directional_fire_rate + 0.5f);
	if (n == 0) return;

	auto f = [&, N = n](UUID my_id) mutable {
		if (life <= 0) {
			scheduler.cancel(my_id);
			return;
		}

		float a = (float)pos.angleTo(section->getPlayerPos());


		Vector2f proj_pos =
			pos +
			Vector2f::createUnitVector(a + PIf / 9) *
			(info.hitbox.w / 2 + info.directional_fire_projectile["radius"]);
		shoot(info.directional_fire_projectile, proj_pos, a);

		proj_pos =
			pos +
			Vector2f::createUnitVector(a - PIf / 9) *
			(info.hitbox.w / 2 + info.directional_fire_projectile["radius"]);
		shoot(info.directional_fire_projectile, proj_pos, a);

		if (--N <= 0) {
			scheduler.cancel(my_id);
		}
	};
	scheduler.every(1 / info.directional_fire_rate, f);

	// >TODO push_anim;
}
std::shared_ptr<Projectile>
FirstBoss::shoot(const nlohmann::json& info, const Vector2f& pos, float dir) noexcept {
	auto proj = std::make_shared<Projectile>(
		info, pos, Vector2f::createUnitVector(dir), false
	);

	proj->collider->onEnter = [proj](Object* obj) {
		if (auto ptr = (Player*)obj; obj->idMask.test(Object::PLAYER)) {
			ptr->hit(proj->getDamage());
			proj->remove();
		}
	};

	projectiles_to_shoot.push_back(proj);
	return proj;
}

void FirstBoss::snipe() noexcept {
	if (info.sniper_shots == 0) return;

	auto fire = [&](auto) {
		section->stopAimAnimation();

		auto dir = Vector2f::createUnitVector(pos.angleTo(section->getPlayerPos()));
		auto proj_pos = support((float)dir.angleX(), info.sniper_projectile["radius"]);
		shoot(
			info.sniper_projectile, proj_pos, (float)pos.angleTo(section->getPlayerPos())
		);
	};
	auto start_aim_anim = [&, N = info.sniper_shots](UUID my_id) mutable {
		section->startAimAnimation();

		scheduler.in(info.sniper_aim_time, fire);

		if (--N == 0) {
			scheduler.cancel(my_id);
		}
	};
	scheduler.every(info.sniper_aim_time + info.sniper_calm_time, start_aim_anim);
}
void FirstBoss::randomFire() noexcept {
	if (info.random_fire_orbs == 0) return;

	std::uniform_real_distribution<float> rngA(
		-info.random_fire_spread * 0.5f, info.random_fire_spread * 0.5f
	);

	//nOrbs * E(rngDt) = time
	std::uniform_real_distribution<float> rngDelay(
		0, 2 * info.random_fire_estimated_time / info.random_fire_orbs
	);

	auto fire = [&](auto) {
		float a = info.random_fire_aim + rngA(C::RD);
		auto dir = Vector2f::createUnitVector(a);
		auto proj_pos = support(a, info.random_fire_projectile["radius"]);

		shoot(info.random_fire_projectile, proj_pos, a);
	};

	UUID previous = scheduler.in(rngDelay(C::RD), fire);

	for (size_t i = 1; i < info.random_fire_orbs; ++i) {
		previous = scheduler.after_in(previous, rngDelay(C::RD), fire);
	}
}