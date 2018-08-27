#pragma once
#include <string>

namespace str {
	extern bool is_whitespace(char c) noexcept;
	extern bool is_numeric(char c) noexcept;

	extern std::string trim_whitespace(std::string_view str) noexcept;
}