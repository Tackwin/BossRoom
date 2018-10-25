#pragma once
#include <memory>
#include <functional>
#include <unordered_set>

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "Math/Vector.hpp"

#include "Graphics/AnimatedSprite.hpp"
#include "Graphics/ParticleGenerator.hpp"

#include "Physics/Object.hpp"

#include "Common.hpp"

#include "Components/Hitable.hpp"
#include "Components/Removable.hpp"

#include "3rd/json.hpp"

#include "Time/Scheduler.hpp"

struct FirstBossInfo {
	static constexpr auto JSON_ID = "FirstBossInfo";

	size_t sniper_shots{ 0 };
	size_t random_fire_orbs{ 0 };

	float max_life{ NAN };
	float directional_fire_rate{ NAN };
	float directional_fire_duration{ NAN };
	float sniper_aim_time{ NAN };
	float sniper_calm_time{ NAN };
	float cd_directional{ NAN };
	float cd_random_fire{ NAN };
	float cd_snipe{ NAN };
	float random_fire_spread{ NAN };
	float random_fire_aim{ NAN };
	float random_fire_estimated_time{ NAN };

	Rectangle2f hitbox;

	Vector2f start_pos;
	Vector2f origin;

	std::string texture;

	nlohmann::json hit_particle;
	nlohmann::json directional_fire_projectile;
	nlohmann::json sniper_projectile;
	nlohmann::json random_fire_projectile;

	static FirstBossInfo loadJson(const nlohmann::json& json) noexcept;
	static nlohmann::json saveJson(const FirstBossInfo& info) noexcept;
};

extern void from_json(const nlohmann::json& json, FirstBossInfo& info) noexcept;
extern void to_json(nlohmann::json& json, const FirstBossInfo& info) noexcept;

class Section;
class Projectile;
class FirstBoss : public Object, public Hitable, public Removable {
public:
	FirstBoss(const FirstBossInfo& info) noexcept;

	FirstBoss(const FirstBoss&) = delete;
	FirstBoss& operator=(const FirstBoss&) = delete;

	void enterSection(Section* section) noexcept;
	void leaveSection() noexcept;

	void update(double dt) noexcept;
	void render(sf::RenderTarget& target) noexcept;

	virtual void hit(float damage) noexcept override;
	virtual bool toRemove() const noexcept override;
	virtual void remove() noexcept override;

	const FirstBossInfo& getInfo() const noexcept;

	const std::vector<std::shared_ptr<Projectile>>& getProtectilesToShoot() const noexcept;
	void clearProtectilesToShoot() noexcept;

	Vector2f support(float a, float d) const noexcept;

private:
	void onEnter(Object* obj) noexcept;

	void removeNeeded() noexcept;

	void directionalFire() noexcept;
	void snipe() noexcept;
	void randomFire() noexcept;

	FirstBossInfo info;

	Scheduler scheduler;
	UUID directional_task{ UUID::zero() };
	UUID snipe_task{ UUID::zero() };
	UUID random_fire_task{ UUID::zero() };

	float life;

	bool removed{ false };
	bool flip_x{ false };

	std::vector<std::shared_ptr<Projectile>> projectiles_to_shoot;

	sf::Sprite sprite;

	std::vector<sf::Sound> sounds;

	Section* section{ nullptr };

	std::vector<std::unique_ptr<ParticleGenerator>> particle_effects;
};