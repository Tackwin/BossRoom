#include "string_algorithms.hpp"

#include <locale>

namespace str {
	bool is_whitespace(char c) noexcept {
		return std::isspace(c, std::locale());
	}

	std::string trim_whitespace(std::string_view str) noexcept {
		std::string result{ str };
		if (str.empty()) return result;

		for (size_t i = str.size(); i > 0; --i) {
			if (is_whitespace(result[i - 1])) {
				result.pop_back();
			}
			else break;
		}
		return result;
	}
};