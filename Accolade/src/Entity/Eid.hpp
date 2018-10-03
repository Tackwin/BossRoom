#pragma once
#include "EntityStore.hpp"
#include <numeric>

template<typename T>
class Eid {
private:
	friend EntityStore;
	template <class U> friend class Eid;

public:

	using point_to_t = T;

	constexpr explicit operator EntityStore::integer_t() const noexcept {
		return ptr;
	}
	constexpr operator bool() const noexcept {
		return ptr != std::numeric_limits<EntityStore::integer_t>::max();
	}

	constexpr bool operator==(const Eid<T>& other) const noexcept {
		return other.ptr == ptr;
	}

	constexpr Eid<const T> to_const() const noexcept {
		return Eid<const std::remove_const_t<T>>(ptr);
	}

	Eid() = default;
private:

	inline static EntityStore::integer_t N =
		std::numeric_limits<EntityStore::integer_t>::min();

	constexpr Eid(const EntityStore::integer_t& ptr) noexcept : ptr(ptr) {}

	EntityStore::integer_t ptr{std::numeric_limits<EntityStore::integer_t>::max()};
};
