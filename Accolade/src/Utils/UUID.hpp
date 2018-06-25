#pragma once
#include <random>
#include "./../Common.hpp"

class UUID {
private:

	char uuid[16];
	friend std::hash<UUID>;

public:

	static const UUID null;

	UUID() {
		std::uniform_int_distribution<int> rng{ 
			(int)std::numeric_limits<char>::min(), 
			(int)std::numeric_limits<char>::max()
		};

		for (int i = 0; i < sizeof(uuid); ++i) {
			uuid[i] = (char)rng(C::RD);
		}
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
};

namespace std {
	template<>
	struct hash<UUID> {
		size_t operator()(const UUID& id) const {
			return	*reinterpret_cast<const unsigned*>(&id.uuid[0]);
		}
	};
}