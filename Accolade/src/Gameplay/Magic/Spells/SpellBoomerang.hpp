#pragma once

#include "Spell.hpp"

#include "Math/Vector.hpp"

#include "Physics/Object.hpp"
#include "Physics/Collider.hpp"

#include "Graphics/ParticleGenerator.hpp"

struct SpellBoomerangInfo {
	static constexpr auto JSON_ID = "SpellBoomerangInfo";

	float speed{ 1.f};
	float range{ 5.0f };
	float damage{ 1.f };
	float radius{ 0.05f };
	float rotateSpeed{ 3.f };

	std::string particleGenerator;

	static SpellBoomerangInfo loadJson(nlohmann::json json) noexcept;
	static nlohmann::json saveJson(SpellBoomerangInfo info) noexcept;
};

class SpellBoomerang : public Spell, public Object {
public:
	SpellBoomerang(Section* section, SpellBoomerangInfo info) noexcept;

	SpellBoomerang(SpellBoomerang&) = delete;
	SpellBoomerang(SpellBoomerang&&) = delete;

	SpellBoomerang& operator=(SpellBoomerang&) = delete;
	SpellBoomerang& operator=(SpellBoomerang&&) = delete;

	virtual void update(double dt) noexcept override;
	virtual void render(sf::RenderTarget& target) noexcept override;

	void launch(std::weak_ptr<Object> obj) noexcept;

	SpellBoomerangInfo getSpellInfo() const noexcept;
private:

	void onEnter(Object* obj) noexcept;
	void onExit(Object* obj) noexcept;

	bool launched_{ false };

	float angleToPlayer_{ 0.f };

	SpellBoomerangInfo info_;

	ParticleGenerator particleGenerator_;

	std::weak_ptr<Object> target_;
};