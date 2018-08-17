#include "UUID.hpp"

const UUID UUID::null;

void to_json(nlohmann::json& json, const UUID& id) {
	std::string str{ std::begin(id.uuid), std::end(id.uuid) };

	json = { {"id", str} };
}

void from_json(const nlohmann::json& json, UUID& id) {
	memcpy(&id.uuid, json.at("id").get<std::string>().c_str(), 16);
}
