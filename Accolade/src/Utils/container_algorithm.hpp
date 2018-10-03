#pragma once

#include <functional>
#include <optional>
#include <vector>

template<typename T> std::optional<T>
find(const std::vector<T>& vec, std::function<bool(const T&)> pred) noexcept {
	for (auto& x : vec) if (pred(x)) return x;
	return {};
}