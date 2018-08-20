#pragma once

#include <SFML/Graphics.hpp>

#include "3rd/json.hpp"

#include "Physics/Object.hpp"

#include "Components/Hitable.hpp"
#include "Components/Removable.hpp"

struct MeleeGuyInfo {
	static constexpr auto JSON_ID = "MeleeGuyInfo";

	float damage{ NAN };
	float viewRange{ NAN };
	float speed{ NAN };
	float cd{ NAN };
	float health{ NAN };

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

	void enterSection(Section* section) noexcept;
	void leaveSection() noexcept;

	void update(double dt) noexcept;
	void render(sf::RenderTarget& target) noexcept;

	virtual void hit(float dmg) noexcept override;

	virtual bool toRemove() const noexcept override;
	virtual void remove() noexcept override;

private:

	float health{ NAN };
	float maxHealth{ NAN };

	bool removed{ false };

	Section* section{ nullptr };

	MeleeGuyInfo info;

};