#pragma once

#include <SFML/Graphics.hpp>

#include "3rd/json.hpp"
#include "RTTI/dyn_struct.hpp"
#include "Math/Rectangle.hpp"
#include "Physics/Object.hpp"

struct SemiPlateformeInfo {
	static constexpr auto JSON_ID = "SemiPlateformeInfo";

	Rectangle2f rec;

	static nlohmann::json saveJson(const SemiPlateformeInfo& p) noexcept;
	static SemiPlateformeInfo loadJson(const nlohmann::json& j) noexcept;
};

extern void to_json(nlohmann::json& j, const SemiPlateformeInfo& p) noexcept;
extern void from_json(const nlohmann::json& j, SemiPlateformeInfo& p) noexcept;

extern void to_dyn_struct(dyn_struct& d_struct, const SemiPlateformeInfo& p) noexcept;
extern void from_dyn_struct(const dyn_struct& d_struct, SemiPlateformeInfo& p) noexcept;

class SemiPlateforme : public Object {
public:
	SemiPlateforme(const SemiPlateformeInfo& info) noexcept;

	void renderDebug(sf::RenderTarget& target) noexcept;
	void render(sf::RenderTarget& target) noexcept;
	void update(double dt) noexcept;


private:
	SemiPlateformeInfo info;
};