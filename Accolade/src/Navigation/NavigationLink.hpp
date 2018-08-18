#pragma once
#include "3rd/json.hpp"

#include "Utils/UUID.hpp"

struct NavigationLinkInfo {
	static constexpr auto JSON_ID = "NavigationLinkInfo";

	UUID id;

	UUID A;
	UUID B;

	static NavigationLinkInfo loadJson(nlohmann::json json) noexcept;
	static nlohmann::json saveJson(NavigationLinkInfo info) noexcept;
};