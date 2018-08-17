#pragma once
#include <random>

#include "3rd/json.hpp"

#include "Common.hpp"

class UUID {
public:

	static const UUID null;
	static constexpr auto SIZE = 16;

	UUID() {
		std::uniform_int_distribution<int> rng{ 
			(int)std::numeric_limits<char>::min(), 
			(int)std::numeric_limits<char>::max()
		};

		for (int i = 0; i < sizeof(uuid); ++i) {
			uuid[i] = (char)rng(C::RD);
		}
	}

	constexpr void nullify() noexcept {
		for (unsigned i = 0; i < sizeof(UUID); ++i) {
			uuid[i] = null.uuid[i];
		}
	}

	constexpr operator bool() const noexcept {
		return *this != null;
	}

	constexpr bool operator==(const UUID& other) const noexcept {
		for (int i = 0; i < sizeof(UUID); ++i) {
			if (uuid[i] != other.uuid[i]) return false;
		}
		return true;
	}
	constexpr bool operator!=(const UUID& other) const noexcept {
		for (int i = 0; i < sizeof(UUID); ++i) {
			if (uuid[i] != other.uuid[i]) return true;
		}
		return false;
	}
	constexpr bool operator<(const UUID& other) const noexcept {
		for (int i = 0; i < sizeof(uuid); ++i) {
			if (uuid[i] != other.uuid[i]) {
				return uuid[i] < other.uuid[i];
			}
		}
		return false;
	}
private:

	friend void to_json(nlohmann::json& json, const UUID& id);
	friend void from_json(const nlohmann::json& json, UUID& id);

	char uuid[SIZE];
	friend std::hash<UUID>;
};

namespace std {
	template<>
	struct hash<UUID> {
		size_t operator()(const UUID& id) const {
			return	*reinterpret_cast<const unsigned*>(&id.uuid[0]);
		}
	};
};

extern void to_json(nlohmann::json& json, const UUID& id);
extern void from_json(const nlohmann::json& json, UUID& id);
