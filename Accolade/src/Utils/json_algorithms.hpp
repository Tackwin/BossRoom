#pragma once
#include "RTTI/dyn_struct.hpp"
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

template<typename T>
std::vector<T> load_json_vector(const dyn_struct& d_struct) noexcept {
	assert(holds_array(d_struct));
	std::vector<T> vec(size(d_struct));
	for (size_t i = 0; i < vec.size(); ++i) {
		vec[i] = T::loadJson(d_struct[i]);
	}
	return vec;
}