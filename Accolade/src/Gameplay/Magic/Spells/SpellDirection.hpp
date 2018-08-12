#pragma once

#include "Spell.hpp"
#include <string>

#include "Physics/Object.hpp"

#include "Graphics/ParticleGenerator.hpp"

#include "Components/Hitable.hpp"

struct SpellDirectionInfo {
	static constexpr auto JSON_ID = "SpellDirectionInfo";

	float speed{ NAN };
	float damage{ NAN };
	float radius{ NAN };
	float lifeTime{ NAN };
	float capacity{ NAN };

	std::string particleGenerator{ "" };

	static nlohmann::json saveJson(SpellDirectionInfo info) noexcept;
	static SpellDirectionInfo loadJson(nlohmann::json json) noexcept;

};

class Section;
class SpellDirection : public Spell, public Object {
public:

	SpellDirection(
		Section* seciton,
		std::weak_ptr<Object> sender,
		SpellDirectionInfo info
	) noexcept;

	virtual void update(double dt) noexcept;
	virtual void render(sf::RenderTarget& target) noexcept;

	void launch(Vector2f dir, std::bitset<Object::SIZE> target) noexcept;

	virtual void remove() noexcept override;

	void setPos(Vector2f pos) noexcept;

	SpellDirectionInfo getInfo() const noexcept;
private:

	void dealCapacity(float damage, Hitable* hitable) noexcept;

	void onEnter(Object* object) noexcept;
	void onExit(Object* object) noexcept;

	float capacity_{ NAN };
	bool launched_{ false };
	float angleToSender_{ 0.f };
	float lifeTimer_{ 0.f };

	SpellDirectionInfo info_;

	ParticleGenerator particleGenerator_;

	UUID senderUuid_;
	std::weak_ptr<Object> sender_;
	Vector2f dir_;
};