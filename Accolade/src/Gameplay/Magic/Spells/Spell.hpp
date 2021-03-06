#pragma once

#include <SFML/Graphics.hpp>

#include "3rd/json.hpp"

#include "Utils/UUID.hpp"

#include "Components/Removable.hpp"

class Section;

class Spell : public Removable {

public:
	Spell(Section* section) noexcept;

	virtual void enter(Section* section) noexcept;
	virtual void leave() noexcept;

	virtual void update(double dt) noexcept = 0;
	virtual void render(sf::RenderTarget& target) noexcept = 0;

	UUID getUuid() const noexcept;

	virtual void remove() noexcept override;
	virtual bool toRemove() const noexcept override;

protected:

	Section* section_{ nullptr };

	UUID id_;

	bool _remove{ false };
};