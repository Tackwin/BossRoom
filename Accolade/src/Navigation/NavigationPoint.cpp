#include "NavigationPoint.hpp"

#define LOAD(x, y) if (auto i = json.find(#x); i != json.end()) info.x = y(*i);
#define SAVE(x, y) json[#x] = y(info.x);

NavigationPointInfo NavigationPointInfo::loadJson(nlohmann::json json) noexcept {
	NavigationPointInfo info;

	LOAD(id, );
	LOAD(range, );
	LOAD(pos, Vector2f::loadJson);
	LOAD(links, [](auto x) {return x->get<std::vector<UUID>>(); });

	return info;
}

nlohmann::json NavigationPointInfo::saveJson(NavigationPointInfo info) noexcept {
	nlohmann::json json;

	SAVE(pos, );
	SAVE(range, );
	SAVE(id, );
	SAVE(links, );

	return json;
}

#undef LOAD;
#undef SAVE;
