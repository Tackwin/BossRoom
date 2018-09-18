#include "string_algorithms.hpp"

#include <locale>

namespace str {
	bool is_whitespace(char c) noexcept {
		return std::isspace(c, std::locale());
	}
	bool is_numeric(char c) noexcept {
		return std::isdigit(c, std::locale());
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
		size_t new_start = 0;
		for (size_t i = 0; i < str.size(); ++i) {
			if (is_whitespace(result[i])) {
				new_start++;
			}
			else break;
		}
		return result.substr(new_start);
	}

	bool represent_number(std::string_view str) noexcept {
		// actually check if it's a resolvable expression.

		if (str.empty()) return false;
		auto minus = (str[0] == '-');

		for (const auto& c : str.substr(minus ? 1 : 0)) {
			//std::unordered_set<char> allowed{ '+', '-', '*', '/', '(', ')', ' ', '.' };

			if (!std::isdigit(c, std::locale{})) return false;
		}
		return !str.empty();
	}

};