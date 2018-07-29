#pragma once

#include <SFML/Graphics.hpp>

#include "3rd/json.hpp"

#include "Utils/UUID.hpp"

#include "Components/Removable.hpp"

class Section;
struct SpellInfo {
	static constexpr auto JSON_ID = "SpellInfo";

	static SpellInfo loadJson(nlohmann::json json) noexcept;
	static nlohmann::json saveJson(SpellInfo info) noexcept;
};

class Spell : public Removable {

public:
	Spell(Section* section) noexcept;

	virtual void enter(Section* section) noexcept;
	virtual void leave() noexcept;

	virtual void update(double dt) noexcept = 0;
	virtual void render(sf::RenderTarget& target) noexcept = 0;

	UUID getUuid() const noexcept;

protected:

	Section* section_{ nullptr };
	SpellInfo info_;

	UUID id_;
};