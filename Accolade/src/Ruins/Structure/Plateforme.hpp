#pragma once
#include "./../../Physics/Collider.hpp"
#include "Structure.hpp"
#include "../../Math/Rectangle.hpp"

struct PlateformeInfo {
	Rectangle2f rectangle;

	static PlateformeInfo loadFromJson(const nlohmann::json& json) noexcept;
};

class Plateforme : public Structure {
public:

	Plateforme(PlateformeInfo info) noexcept;

	void update(double dt) noexcept override;
	void render(sf::RenderTarget& target) noexcept override;
	void renderDebug(sf::RenderTarget& target) noexcept override;
};