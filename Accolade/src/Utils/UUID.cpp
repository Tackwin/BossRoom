#include "UUID.hpp"

unsigned long long UUID::count = 0ULL;

void to_json(nlohmann::json& json, const UUID& id) {
	json = id.uuid;
}

void from_json(const nlohmann::json& json, UUID& id) {
	id.uuid = json.get<unsigned long long>();
}
