#pragma once
#include "./../../Physics/Collider.hpp"
#include "Structure.hpp"
#include "../../Math/Rectangle.hpp"

struct PlateformeInfo {
	static constexpr auto JSON_ID = "PlateformeInfo";

	Rectangle2f rectangle;

	static PlateformeInfo loadJson(const nlohmann::json& json) noexcept;
	static nlohmann::json saveJson(const PlateformeInfo& info) noexcept;
};

class Plateforme : public Structure {
public:

	Plateforme(PlateformeInfo info) noexcept;

	void update(double dt) noexcept override;
	void render(sf::RenderTarget& target) noexcept override;
	void renderDebug(sf::RenderTarget& target) noexcept override;

	Kind getType() const noexcept override { return Kind::Plateforme; }

	Rectangle2f getBoundingBox() const noexcept;

private:
	PlateformeInfo _info;
};