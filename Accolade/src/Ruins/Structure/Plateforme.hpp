#pragma once
#include "./../../Physics/Collider.hpp"
#include "Structure.hpp"

class Plateforme : public Structure {
public:

	Plateforme(Vector2f pos, Vector2f size) noexcept;

	void update(double dt) noexcept override;
	void render(sf::RenderTarget& target) noexcept override;
	void renderDebug(sf::RenderTarget& target) noexcept override;
};