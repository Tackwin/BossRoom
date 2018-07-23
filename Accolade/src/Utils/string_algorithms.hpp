#pragma once
#include <string>

namespace str {
	extern bool is_whitespace(char c) noexcept;

	extern std::string trim_whitespace(std::string_view str) noexcept;
}