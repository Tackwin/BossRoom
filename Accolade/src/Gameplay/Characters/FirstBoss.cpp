#include "FirstBoss.hpp"

#include <algorithm>
#include <type_traits>

#include "Managers/AssetsManager.hpp"
#include "Gameplay/Projectile.hpp"
#include "Ruins/Section.hpp"
#include "Utils/meta_algorithms.hpp"

void from_json(const nlohmann::json& json, FirstBossInfo& info) noexcept {
	info.hitbox = json.at("hitbox");
	info.max_life = json.at("max_life");
	info.start_pos = json.at("start_pos");
	info.origin = json.at("origin");
	info.hit_particle = json.at("hit_particle");
}

void to_json(nlohmann::json& json, const FirstBossInfo& info) noexcept {
	json["max_life"] = info.max_life;
	json["start_pos"] = info.start_pos;
	json["hitbox"] = info.hitbox;
	json["origin"] = info.origin;
	json["hit_particle"] = info.hit_particle;
}

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
	for (auto& particles : particle_effects) {
		particles->update(dt);
	}

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

		float a =
			(float)pos.angleTo(section->getPlayerPos());

		Vector2f dir = Vector2f::createUnitVector(a);

		Vector2f proj_pos =
			pos +
			Vector2f::createUnitVector(a + PIf / 9) *
			(info.hitbox.w / 2 + 10);

		auto proj = std::make_shared<Projectile>(
			info.directional_fire_projectile, proj_pos, dir, false
		);
		projectiles_to_shoot.push_back(proj);

		proj_pos =
			pos +
			Vector2f::createUnitVector(a - PIf / 9) *
			(info.hitbox.w / 2 + 10);

		proj = std::make_shared<Projectile>(
			info.directional_fire_projectile, proj_pos, dir, false
		);
		projectiles_to_shoot.push_back(proj);

		if (--N <= 0) {
			scheduler.cancel(my_id);
		}
	};
	scheduler.every(1 / info.directional_fire_rate, f);

	// >TODO push_anim;
}

void FirstBoss::snipe() noexcept {
	if (info.sniper_shots == 0) return;

	auto shoot = [&](auto) {
		section->stopAimAnimation();

		auto dir = Vector2f::createUnitVector(pos.angleTo(section->getPlayerPos()));
		auto proj_pos = support((float)dir.angleX(), info.sniper_projectile["radius"]);
		auto proj = std::make_shared<Projectile>(
			info.sniper_projectile, proj_pos, dir, false
		);
		projectiles_to_shoot.push_back(proj);
	};
	auto start_aim_anim = [&, N = info.sniper_shots](UUID my_id) mutable {
		section->startAimAnimation();

		scheduler.in(info.sniper_aim_time, shoot);

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

	auto shoot = [&](auto) {
		float a = info.random_fire_aim + rngA(C::RD);
		auto dir = Vector2f::createUnitVector(a);
		auto proj_pos = support(a, info.random_fire_projectile["radius"]);

		auto proj = std::make_shared<Projectile>(
			info.random_fire_projectile, proj_pos, dir, false
		);
	};

	UUID previous = scheduler.in(rngDelay(C::RD), shoot);

	for (size_t i = 1; i < info.random_fire_orbs; ++i) {
		previous = scheduler.after_in(previous, rngDelay(C::RD), shoot);
	}
}