#pragma once
#include <SFML/Graphics.hpp>

#include "3rd/json.hpp"

#include "Math/Vector.hpp"

#include "Physics/Object.hpp"

#include "Components/Removable.hpp"
#include "Components/Hitable.hpp"

struct DistanceGuyInfo {
	static constexpr auto JSON_ID = "Distance";

	Vector2f startPos;
	Vector2f origin;
	Vector2f size;
	float health{ 10.f };
	float range{ 10.f };
	float maxHealth{ 10.f };
	float reloadTime{ 1.5f };
	float lifeTimeBoost{ 2.f };
	float contactDamage{ NAN };
	float spellDamage{ NAN };
	std::string sprite{ "distanceGuy" };

	static DistanceGuyInfo loadJson(nlohmann::json json) noexcept;
	static nlohmann::json saveJson(DistanceGuyInfo info) noexcept;
};

class Section;
class DistanceGuy : public Removable, public Object, public Hitable {

public:

	DistanceGuy(DistanceGuyInfo info) noexcept;

	void enterSection(Section* section) noexcept;

	void update(double dt) noexcept;
	void render(sf::RenderTarget& target) noexcept;

	virtual void hit(float damage) noexcept override;

	Vector2f support(float a, float d) const noexcept;

	virtual void remove() noexcept override;
	virtual bool toRemove() const noexcept override;

private:

	void onEnter(Object* obj) noexcept;
	void onExit(Object* obj) noexcept;

	bool _remove{ false };

	float reloadTimer_{ 0.f };


	Vector4f colorToAttack_{ 1.f, 1.f, 1.f, 1.f };
	Section * section_{ nullptr };
	DistanceGuyInfo info_;
	sf::Sprite sprite_;
};