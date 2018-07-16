#pragma once
#include <SFML/Graphics.hpp>
#include "3rd/json.hpp"
#include "Physics/Object.hpp"
#include "Graphics/AnimatedSprite.hpp"

class Section;

struct SlimeInfo {

	Vector2f startPos{ 0., 0. };
	float speed{ 1.f };
	std::string sprite{ "slime" };
	Vector2f size{ 1.f, 1.f };

	static SlimeInfo loadFromJson(nlohmann::json json) noexcept;
};

class Slime : public Object {
public:
	Slime(SlimeInfo info) noexcept;

	void enterSection(Section* section) noexcept;

	void update(double dt) noexcept;
	void render(sf::RenderTarget& target) noexcept;

	void onEnter(Object* collider) noexcept;
	void onExit(Object* collider) noexcept;

private:
	SlimeInfo _info;

	Section* _section{ nullptr };

	sf::Sprite _sprite;
};