#include "Portal.hpp"

#define SAVE(x, y) json[#x] = y(info.x);

nlohmann::json PortalInfo::saveJson(PortalInfo info) noexcept {
	nlohmann::json json;

	SAVE(frontier, );

	SAVE(id, );
	SAVE(spot, );
	SAVE(tp_to, );
	SAVE(drop_pos, );

	return json;
}

#undef SAVE
#define LOAD(x, y) if (auto i = json.find(#x); i != json.end()) info.x = y(*i);

PortalInfo PortalInfo::loadJson(nlohmann::json json) noexcept {
	PortalInfo info;

	LOAD(frontier, );

	LOAD(id, );
	LOAD(spot, );
	LOAD(tp_to, );
	LOAD(drop_pos, );

	return info;
}

#undef LOAD

bool is_in_portal(const PortalInfo& p, const Rectangle2f& rec) noexcept {
	return p.frontier.intersect(rec);
}


void render(const PortalInfo& p, sf::RenderTarget& target) noexcept {
	// TODO
	p.frontier.render(target, { 1, 1, 1, 1 });
}
