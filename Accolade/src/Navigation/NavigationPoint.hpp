#pragma once
#include "3rd/json.hpp"

#include "Math/Vector.hpp"

#include "Utils/UUID.hpp"

struct NavigationPointInfo {
	static constexpr auto JSON_ID = "NavigationPointInfo";

	Vector2f pos;
	float range{ NAN };
	UUID id{ UUID::zero() };
	std::vector<UUID> links;

	static NavigationPointInfo loadJson(nlohmann::json json) noexcept;
	static nlohmann::json saveJson(NavigationPointInfo info) noexcept;
};
