#pragma once
#include <numeric>
#include <type_traits>

#include "3rd/json.hpp"

#include "EntityStore.hpp"

template<typename T>
class OwnId;

template<typename T>
class Eid {
private:
	friend EntityStore;
	template <class U> friend class Eid;
	friend class OwnId<T>;

	friend void to_json<T>(nlohmann::json&, const Eid<T>&) noexcept;
	friend void from_json<T>(const nlohmann::json&, Eid<T>&) noexcept;

public:

	using point_to_t = T;

	constexpr explicit operator EntityStore::integer_t() const noexcept {
		return ptr;
	}
	constexpr explicit operator OwnId<T>() const noexcept {
		return OwnId<T>{ptr};
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

template<typename T>
void to_json(nlohmann::json& json, const Eid<T>& x) noexcept {
	json = x.ptr;
}
template<typename T>
void from_json(const nlohmann::json& json, Eid<T>& x) noexcept {
	x.ptr = json.get<EntityStore::integer_t>();
}
