#pragma once
#include <SFML/Graphics.hpp>

#include "./../../Physics/Object.hpp"

class Structure : public Object {
public:
	enum Kind {
		Plateforme,
		Count
	};

	virtual void update(double dt) noexcept = 0;
	virtual void render(sf::RenderTarget& target) noexcept = 0;
	virtual void renderDebug(sf::RenderTarget& target) noexcept = 0;

	// TOSEE
	// I probably want to move that to Rectangle2f
	virtual Kind getType() const noexcept = 0;
};