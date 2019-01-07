#pragma once

#include <functional>
#include <optional>
#include <vector>
#include <unordered_map>

template<typename T> const T*
find(const std::vector<T>& vec, std::function<bool(const T&)> pred) noexcept {
	for (const T& x : vec) if (pred(x)) return std::addressof(x);
	return nullptr;
}
template<typename T> T*
find(std::vector<T>& vec, std::function<bool(const T&)> pred) noexcept {
	for (T& x : vec) if (pred(x)) return std::addressof(x);
	return nullptr;
}

template<typename K, typename V>
std::vector<V*> reservoir_sampling_predicated(
	std::unordered_map<K, V>& cont,
	const std::function<bool(const V&)>& pred,
	size_t reservoir_size,
	std::default_random_engine rd
) noexcept {
	std::vector<V*> reservoir;

	size_t i = 0;
	for (auto&[k, v] : cont) {
		if (!pred(v)) continue;

		if (reservoir.size() < reservoir_size) {
			reservoir.push_back(std::addressof(v));
			continue;
		}

		size_t rand_idx = std::uniform_int_distribution<size_t>{ 0, i }(rd);
		if (rand_idx < reservoir_size) {
			reservoir[rand_idx] = std::addressof(v);
		}
		++i;
	}
	return reservoir;
}
