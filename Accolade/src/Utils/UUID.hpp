#pragma once
#include <random>

#include "3rd/json.hpp"

#include "Common.hpp"

class UUID {
public:

	constexpr UUID() noexcept : uuid(count++) {
		// check for overflow
		assert(count != 0ULL);
	}

	constexpr void nullify() noexcept {
		uuid = 0;
	}

	constexpr operator bool() const noexcept {
		return *this != UUID{ 0 };
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
	constexpr UUID(unsigned long long n) noexcept : uuid(n) {}

	static unsigned long long count;

	friend void to_json(nlohmann::json& json, const UUID& id);
	friend void from_json(const nlohmann::json& json, UUID& id);

	unsigned long long uuid;
	friend std::hash<UUID>;
public:
	static constexpr UUID zero() noexcept {
		return { 0 };
	}

};

namespace std {
	template<>
	struct hash<UUID> {
		constexpr size_t operator()(const UUID& id) const {
			return (size_t)id.uuid;
		}
	};
};

extern void to_json(nlohmann::json& json, const UUID& id);
extern void from_json(const nlohmann::json& json, UUID& id);
extern void print(const UUID& id) noexcept;
