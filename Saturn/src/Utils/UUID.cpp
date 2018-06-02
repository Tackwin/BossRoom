#include "Utils/UUID.hpp"

#include <random>

#include "Common.hpp"

UUID::UUID() noexcept {
	std::default_random_engine rng(seed);
	std::uniform_int<int> dist(0, 16);
	_str = "xxxxxxxx-xxxx-4xxx-yxxx-xxxxxxxxxxxx";

	for (auto& c : _str) {
		const auto r = dist(rng);
		if (c == 'x') {
			c = r;
		}
		else if (c == 'y') {
			c = r & 0x3 | 0x8;
		}
	}
}

UUID::operator std::string() const noexcept {
	return _str;
}
bool UUID::operator==(const UUID& other) const noexcept {
	return other._str == _str;
}
bool UUID::operator!=(const UUID& other) const noexcept {
	return !this->operator==(other);
}