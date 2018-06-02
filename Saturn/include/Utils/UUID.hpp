#pragma once
#include <string>

class UUID {
	std::string _str;

public:
	UUID() noexcept;

	explicit operator std::string() const noexcept;
	bool operator==(const UUID& other) const noexcept;
	bool operator!=(const UUID& other) const noexcept;
};