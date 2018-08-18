#pragma once
#include <random>

#include "3rd/json.hpp"

#include "Common.hpp"

class UUID {
public:

	static const UUID null;
	static constexpr auto SIZE = 16;

	UUID() {
		static unsigned long long count = 0;

		uuid = count++;

		// check for overflow
		assert(count != 0);
	}

	constexpr void nullify() noexcept {
		uuid = 0;
	}

	constexpr operator bool() const noexcept {
		return *this != null;
	}
	
	constexpr bool operator==(const UUID& other) const noexcept {
		return uuid == other.uuid;
	}
	constexpr bool operator!=(const UUID& other) const noexcept {
		return uuid != other.uuid;
	}
	constexpr bool operator<(const UUID& other) const noexcept {
		return uuid < other.uuid;
	}
private:
	static UUID zero() noexcept;

	friend void to_json(nlohmann::json& json, const UUID& id);
	friend void from_json(const nlohmann::json& json, UUID& id);

	unsigned long long uuid;
	friend std::hash<UUID>;
};

namespace std {
	template<>
	struct hash<UUID> {
		size_t operator()(const UUID& id) const {
			return (size_t)id.uuid;
		}
	};
};

extern void to_json(nlohmann::json& json, const UUID& id);
extern void from_json(const nlohmann::json& json, UUID& id);
