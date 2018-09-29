#pragma once
#include "EntityStore.hpp"
#include <numeric>

template<typename T>
class Eid {
public:


	using point_to_t = T;

	constexpr explicit operator EntityStore::integer_t() const {
		return ptr;
	}
	constexpr operator bool() const {
		return ptr != std::numeric_limits<EntityStore::integer_t>::max();
	}

private:
	friend EntityStore;

	inline static EntityStore::integer_t N =
		std::numeric_limits<EntityStore::integer_t>::min();

	Eid() = default;
	constexpr Eid(const EntityStore::integer_t& ptr) noexcept : ptr(ptr) {}

	EntityStore::integer_t ptr{std::numeric_limits<EntityStore::integer_t>::max()};
};
