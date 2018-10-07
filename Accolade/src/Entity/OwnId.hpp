#pragma once
#include <numeric>
#include <type_traits>

#include "3rd/json.hpp"
#include "EntityStore.hpp"
#include "Entity/Eid.hpp"
#include "Common.hpp"


template<typename T>
class OwnId {
private:

	static void force_to_instantiate_eid(){
		Eid<T>;
	}

	friend EntityStore;
	template<typename U> friend class OwnId;

	friend void to_json(nlohmann::json&, const OwnId<T>&);
	friend void from_json(const nlohmann::json&, OwnId<T>&);
public:

	using point_to_t = T;

	constexpr explicit operator EntityStore::integer_t() const noexcept {
		return ptr;
	}
	constexpr explicit operator const Eid<T>() const noexcept {
		return Eid<T>(ptr);
	}
	constexpr operator bool() const noexcept {
		return ptr != std::numeric_limits<EntityStore::integer_t>::max();
	}

	constexpr bool operator==(const OwnId<T>& other) const noexcept {
		return other.ptr == ptr;
	}

	constexpr OwnId<const T> to_const() const noexcept {
		return OwnId<const std::remove_const_t<T>>(ptr);
	}

	OwnId() = default;
	constexpr OwnId(Eid<T>&& id) noexcept : ptr(id.ptr) {};
	~OwnId() noexcept {
		es_instance->destroy(*this);
	}

	// TOSEE i don't know if i must make a deep copy or forbid copy...
	constexpr OwnId(
		std::enable_if_t<std::is_copy_constructible_v<T>, OwnId<T>>& other
	) noexcept {
		ptr = es_instance->copy<T>(es_instance->get<T>(other.ptr));
	}
	constexpr OwnId(OwnId<T>&& other) noexcept : ptr(other.ptr) {
		other.ptr = NULL_PTR;
	}

	constexpr OwnId& operator=(
		std::enable_if_t<std::is_copy_constructible_v<T>, OwnId<T>>& other
	) noexcept {
		ptr = es_instance->copy<T>(es_instance->get<T>(other.ptr));
		return *this;
	}
	constexpr OwnId& operator=(OwnId<T>&& other) noexcept {
		ptr = other.ptr;

		other.ptr = NULL_PTR;
		return *this;
	}
private:

	inline static EntityStore::integer_t N =
		std::numeric_limits<EntityStore::integer_t>::min();
	constexpr static EntityStore::integer_t NULL_PTR =
		std::numeric_limits<EntityStore::integer_t>::min();

	constexpr OwnId(const EntityStore::integer_t& ptr) noexcept : ptr(ptr) {}

	EntityStore::integer_t ptr{ std::numeric_limits<EntityStore::integer_t>::max() };
};


template<typename T>
void to_json(nlohmann::json& json, const OwnId<T>& x) noexcept {
	json = x.ptr;
}
template<typename T>
void from_json(const nlohmann::json& json, OwnId<T>& x) noexcept {
	x.ptr = json.get<EntityStore::integer_t>();
}

