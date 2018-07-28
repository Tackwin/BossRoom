#pragma once

#include "Spell.hpp"

#include "Math/Vector.hpp"

#include "Physics/Object.hpp"

struct SpellBoomerangInfo {
	static constexpr auto JSON_ID = "SpellBoomerangInfo";

	float speed{ 1.f};

	static SpellBoomerangInfo loadJson(nlohmann::json json) noexcept;
	static nlohmann::json saveJson(SpellBoomerangInfo info) noexcept;
};

class SpellBoomerang : public Spell {
public:
	SpellBoomerang(
		Section* section, std::shared_ptr<Object> target, SpellBoomerangInfo info
	) noexcept;

	virtual void update(double dt) noexcept override;
	virtual void render(sf::RenderTarget& target) noexcept override;

private:
	
	SpellBoomerangInfo info_;

	std::shared_ptr<Object> target_;
	Vector2f pos_;
};