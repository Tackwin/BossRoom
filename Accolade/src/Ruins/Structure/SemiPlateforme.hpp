#pragma once
#include "3rd/json.hpp"
#include "Math/Rectangle.hpp"

struct SemiPlateforme {
	static constexpr auto JSON_ID = "SemiPlateform";

	Rectangle2f rec;

	static nlohmann::json saveJson(const SemiPlateforme& p) noexcept;
	static SemiPlateforme loadJson(const nlohmann::json& j) noexcept;
};