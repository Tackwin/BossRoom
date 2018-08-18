#include "UUID.hpp"

const UUID UUID::null = UUID::zero();

UUID UUID::zero() noexcept {
	static UUID i;
	i.uuid = 0;
	return i;
}

void to_json(nlohmann::json& json, const UUID& id) {
	json = id.uuid;
}

void from_json(const nlohmann::json& json, UUID& id) {
	id.uuid = json.get<unsigned long long>();
}
