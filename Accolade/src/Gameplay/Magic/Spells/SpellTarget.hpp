#pragma once

#include "Spell.hpp"

#include "Math/Vector.hpp"

#include "Physics/Object.hpp"
#include "Physics/Collider.hpp"

#include "Graphics/ParticleGenerator.hpp"

struct SpellTargetInfo {
	static constexpr auto JSON_ID = "SpellTargetInfo";

	float speed{ 1.f};
	float range{ 5.0f };
	float damage{ 1.f };
	float radius{ 0.05f };
	float rotateSpeed{ 3.f };

	std::string particleGenerator;

	static SpellTargetInfo loadJson(nlohmann::json json) noexcept;
	static nlohmann::json saveJson(SpellTargetInfo info) noexcept;
};

class SpellTarget : public Spell, public Object {
public:
	SpellTarget(
		Section* section, std::weak_ptr<Object> sender, SpellTargetInfo info
	) noexcept;

	SpellTarget(SpellTarget&) = delete;
	SpellTarget(SpellTarget&&) = delete;

	SpellTarget& operator=(SpellTarget&) = delete;
	SpellTarget& operator=(SpellTarget&&) = delete;

	virtual void update(double dt) noexcept override;
	virtual void render(sf::RenderTarget& target) noexcept override;

	void launch(std::weak_ptr<Object> obj) noexcept;

	SpellTargetInfo getSpellInfo() const noexcept;
private:

	void onEnter(Object* obj) noexcept;
	void onExit(Object* obj) noexcept;

	bool launched_{ false };

	float angleToSender_{ 0.f };

	SpellTargetInfo info_;

	ParticleGenerator particleGenerator_;

	std::weak_ptr<Object> target_;
	std::weak_ptr<Object> sender_;
};