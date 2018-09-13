#pragma once

#include <SFML/Graphics.hpp>

#include "Utils/UUID.hpp"

#include "3rd/json.hpp"

#include "Math/Segment.hpp"
#include "Math/Rectangle.hpp"


struct PortalInfo {
	static constexpr auto JSON_ID = "PortalInfo";

	Segment2f frontier;

	bool start{ false };
	bool end{ false };

	UUID tp_to{ UUID::zero() };
	UUID id;

	static nlohmann::json saveJson(PortalInfo info) noexcept;
	static PortalInfo loadJson(nlohmann::json json) noexcept;
};

extern bool is_in_portal(const PortalInfo& p, const Rectangle2f& rec) noexcept;
extern void render(const PortalInfo& p, sf::RenderTarget& target) noexcept;