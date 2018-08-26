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

	std::string sprite;

	Vector2f startPos;
	Vector2f size;

	float speed{ NAN };
	float health{ NAN };
	float contactDamage{ NAN };
	float viewRange{ NAN };
	float maxHealth{ NAN };

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

	void attachTo(NavigationPointInfo point) noexcept;

private:
	void walkToward() noexcept;

	SlimeInfo _info;

	Section* _section{ nullptr };

	NavigationPointInfo currentPoint_;

	AnimatedSprite sprite;

	bool _remove{ false };
	bool faceX{ true };
};