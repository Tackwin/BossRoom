#pragma once
#include "3rd/json.hpp"

#include <vector>

extern nlohmann::json::object_t merge(
	const nlohmann::json::object_t& a, const nlohmann::json::object_t& b
) noexcept;

template<typename T>
std::vector<T> load_json_vector(const nlohmann::json::array_t& json) noexcept {
	std::vector<T> vec;
	vec.reserve(json.size());
	for (auto t : json) {
		vec.push_back(typename T::loadJson(t));
	}
	return vec;
}