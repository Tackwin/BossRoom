#include "NavigationLink.hpp"

#define LOAD(x, y) if (auto i = json.find(#x); i != json.end()) info.x = y(*i);
#define SAVE(x, y) json[#x] = y(info.x);

NavigationLinkInfo NavigationLinkInfo::loadJson(nlohmann::json json) noexcept {
	NavigationLinkInfo info;

	LOAD(id, );
	LOAD(A, );
	LOAD(B, );

	return info;
}

nlohmann::json NavigationLinkInfo::saveJson(NavigationLinkInfo info) noexcept {
	nlohmann::json json;

	SAVE(A, );
	SAVE(B, );
	SAVE(id, );

	return json;
}

#undef LOAD
#undef SAVE
