#include "SemiPlateforme.hpp"

#define SAVE(x, y) json[#x] = y(info.x);

nlohmann::json SemiPlateforme::saveJson(const SemiPlateforme& info) noexcept {
	nlohmann::json json;
	SAVE(rec, Rectangle2f::saveJson);
	return json;
}
#undef SAVE
#define LOAD(x, y) if (auto i = json.find(#x); i != json.end()) info.x = y(*i);

SemiPlateforme SemiPlateforme::loadJson(const nlohmann::json& json) noexcept {
	SemiPlateforme info;
	LOAD(rec, Rectangle2f::loadJson);
	return info;
}

#undef LOAD