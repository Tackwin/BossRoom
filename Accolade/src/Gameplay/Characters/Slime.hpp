#pragma once
#include <SFML/Graphics.hpp>
#include "3rd/json.hpp"

#include "Physics/Object.hpp"

#include "Graphics/AnimatedSprite.hpp"

#include "Components/Removable.hpp"
#include "Components/Hitable.hpp"

#include "Navigation/NavigationPoint.hpp"

class Section;

struct SlimeInfo {
	static constexpr auto JSON_ID = "Slime";

	Vector2f startPos{ 0., 0. };
	float speed{ 1.f };
	float health{ 10.f };
	float contactDamage{ NAN };
	float viewRange{ 7.f };
	float maxHealth{ 10.f };
	std::string sprite{ "slime" };
	Vector2f size{ 1.f, 1.f };

	static SlimeInfo loadJson(nlohmann::json json) noexcept;
	static nlohmann::json saveJson(SlimeInfo info) noexcept;
};

class Slime : public Object, public Removable, public Hitable {
public:
	Slime(SlimeInfo info) noexcept;

	void enterSection(Section* section) noexcept;

	void update(double dt) noexcept;
	void render(sf::RenderTarget& target) noexcept;

	void onEnter(Object* collider) noexcept;
	void onExit(Object* collider) noexcept;

	virtual void hit(float damage) noexcept override;

	virtual void remove() noexcept override;
	virtual bool toRemove() const noexcept override;

	void attachTo(NavigationPoint* point) noexcept;

private:
	void walkToward() noexcept;

	SlimeInfo _info;

	Section* _section{ nullptr };

	NavigationPoint* currentPoint_{ nullptr };

	sf::Sprite _sprite;

	bool _remove{ false };
};