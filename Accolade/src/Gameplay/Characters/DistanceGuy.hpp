#pragma once
#include <SFML/Graphics.hpp>

#include "3rd/json.hpp"

#include "Math/Vector.hpp"

#include "Physics/Object.hpp"

#include "Components/Removable.hpp"

struct DistanceGuyInfo {
	static constexpr auto JSON_ID = "Distance";

	Vector2f startPos;
	Vector2f origin;
	Vector2f size;
	float health{ 10.f };
	float range{ 10.f };
	float maxHealth{ 10.f };
	float reloadTime{ 1.5f };
	std::string sprite{ "distanceGuy" };

	static DistanceGuyInfo loadJson(nlohmann::json json) noexcept;
	static nlohmann::json saveJson(DistanceGuyInfo info) noexcept;
};

class Section;
class DistanceGuy : public Removable, public Object {

public:

	DistanceGuy(DistanceGuyInfo info) noexcept;

	void enterSection(Section* section) noexcept;

	void update(double dt) noexcept;
	void render(sf::RenderTarget& target) noexcept;

	void hit(float damage) noexcept;

	Vector2f support(float a, float d) const noexcept;

private:

	void onEnter(Object* obj) noexcept;
	void onExit(Object* obj) noexcept;

	float reloadTimer_{ 0.f };

	Section * section_{ nullptr };
	DistanceGuyInfo info_;
	sf::Sprite sprite_;
};