#include "json_algorithms.hpp"

nlohmann::json::object_t merge(
	const nlohmann::json::object_t& a, const nlohmann::json::object_t& b
) noexcept {

	nlohmann::json::object_t merged{ a };

	for (const auto&[key, value] : b) {
		auto it = merged.find(key);
		
		if (it == merged.end()) {
			merged[key] = value;
			continue;
		}

		if (!it->second.is_object()) {
			merged[key] = value;
			continue;
		}

		merged[key] = merge(merged[key], value);
	}

	return merged;
}