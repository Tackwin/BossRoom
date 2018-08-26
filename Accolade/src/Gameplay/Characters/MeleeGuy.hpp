#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

#include "3rd/json.hpp"

#include "Physics/Object.hpp"

#include "Gameplay/Zone.hpp"

#include "Components/Hitable.hpp"
#include "Components/Removable.hpp"

#include "Navigation/NavigationPoint.hpp"

struct MeleeGuyInfo {
	static constexpr auto JSON_ID = "MeleeGuyInfo";

	Vector2f attackOrigin;
	Vector2f startPos;
	Vector2f origin;
	Vector2f size;

	float damage{ NAN };
	float viewRange{ NAN };
	float speed{ NAN };
	float cd{ NAN };
	float health{ NAN };
	float attackRange{ NAN };
	float attackRadius{ NAN };
	float attackPersistence{ NAN };

	static MeleeGuyInfo loadJson(nlohmann::json json) noexcept;
	static nlohmann::json saveJson(MeleeGuyInfo info) noexcept;
};

class Section;
class MeleeGuy : public Object, public Hitable, public Removable {
public:

	MeleeGuy(MeleeGuyInfo info) noexcept;

	MeleeGuy(MeleeGuy&) = delete;
	MeleeGuy& operator=(MeleeGuy&) = delete;

	MeleeGuy(MeleeGuy&&) = delete;
	MeleeGuy& operator=(MeleeGuy&&) = delete;

	void attachTo(NavigationPointInfo point) noexcept;

	void enterSection(Section* section) noexcept;
	void leaveSection() noexcept;

	void update(double dt) noexcept;
	void render(sf::RenderTarget& target) noexcept;

	virtual void hit(float dmg) noexcept override;

	virtual bool toRemove() const noexcept override;
	virtual void remove() noexcept override;

	const std::vector<std::shared_ptr<Zone>>& getZonesToApply() const noexcept;
	void clearZones() noexcept;
private:

	void onEnter(Object* object) noexcept;
	void onExit(Object* object) noexcept;

	void walkToward() noexcept;

	void attack() noexcept;

	float cdTime = 0.f;
	float attackTime = 0.f;

	float health{ NAN };
	float maxHealth{ NAN };

	bool removed{ false };

	Section* section{ nullptr };

	MeleeGuyInfo info;

	NavigationPointInfo currentPoint;

	sf::Sprite sprite;

	std::vector<std::shared_ptr<Zone>> zones;
};