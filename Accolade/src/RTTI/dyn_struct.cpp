#include "dyn_struct.hpp"

#include <cassert>
#include <functional>
#include "Unicode/Unicode.hpp"

#include "OS/FileIO.hpp"

dyn_struct::dyn_struct(
	std::initializer_list<std::pair<std::string, dyn_struct>> list
) noexcept {
	structure_t str;
	for (auto&[k, v] : list) {
		str[k] = ValuePtr(new dyn_struct(v));
	}
	value = str;
}

dyn_struct* dyn_struct::clone() noexcept {
	return new dyn_struct(*this);
}

ValuePtr<dyn_struct>& dyn_struct::operator[](std::string_view str) noexcept {
	assert(std::holds_alternative<structure_t>(value));

	auto& struc = std::get<structure_t>(value);
	if (struc.count(std::string(str)) == 0) {
		struc[std::string(str)] = ValuePtr(new dyn_struct());
	}

	return struc.at(std::string(str));
}
ValuePtr<dyn_struct>& dyn_struct::operator[](size_t idx) noexcept {
	assert(std::holds_alternative<array_t>(value));
	assert(std::get<array_t>(value).size() > idx);

	auto& arr = std::get<array_t>(value);
	return arr[idx];
}

void dyn_struct::push_back(const dyn_struct& v) noexcept {
	assert(std::holds_alternative<array_t>(value));

	std::get<array_t>(value).push_back({ ValuePtr(new dyn_struct(v)) });
}

void dyn_struct::pop_back() noexcept {
	assert(std::holds_alternative<array_t>(value));
	assert(!std::get<array_t>(value).empty());
	std::get<array_t>(value).pop_back();
}

dyn_struct& set(std::string_view str, const dyn_struct& value, dyn_struct& to) noexcept {
	assert(std::holds_alternative<dyn_struct::structure_t>(to.value));
	std::get<dyn_struct::structure_t>(to.value)
		.emplace(std::string(str), ValuePtr(new dyn_struct(value)));
	return to;
}

#define X(x)\
void to_dyn_struct(dyn_struct& to, const dyn_struct::x& from) noexcept {\
	dyn_struct::variant v;\
	v = from;\
	to.value = std::move(v);\
	to.type_hash = typeid(from).hash_code();\
}\
void from_dyn_struct(const dyn_struct& from, dyn_struct::x& to) noexcept {\
	to = std::get<dyn_struct::x>(from.value);\
}\

X(integer_t)
X(real_t)
X(string_t)
X(boolean_t)
X(structure_t)
X(array_t)

#undef X

#define X(x)\
void to_dyn_struct(dyn_struct& to, const x& from) noexcept {\
	dyn_struct::variant v;\
	v = dyn_struct::integer_t(from);\
	to.value = std::move(v);\
	to.type_hash = typeid(from).hash_code();\
}\
void from_dyn_struct(const dyn_struct& from, x& to) noexcept {\
	to = x(std::get<dyn_struct::integer_t>(from.value));\
}\

X(int)
X(long)
X(unsigned)
X(long unsigned)
X(long long unsigned)
X(short)
X(unsigned short)

#undef X

#define X(x)\
void to_dyn_struct(dyn_struct& to, const x& from) noexcept {\
	dyn_struct::variant v;\
	v = dyn_struct::real_t(from);\
	to.value = std::move(v);\
	to.type_hash = typeid(from).hash_code();\
}\
void from_dyn_struct(const dyn_struct& from, x& to) noexcept {\
	to = x(std::get<dyn_struct::integer_t>(from.value));\
}\

X(float)
X(double)

#undef X

void to_dyn_struct(dyn_struct& to, const dyn_struct::variant& from) noexcept {
	dyn_struct::variant v;
	v = from;
	to.value = std::move(v);
	to.type_hash = typeid(from).hash_code();
}
void from_dyn_struct(const dyn_struct& from, dyn_struct::variant& to) noexcept {
	to = from.value;
}
void to_dyn_struct(dyn_struct& to, const dyn_struct::null_t& from) noexcept {
	dyn_struct::variant v;
	v = from;
	to.value = std::move(v);
	to.type_hash = typeid(from).hash_code();
}
void from_dyn_struct(const dyn_struct&, dyn_struct::null_t&) noexcept {}

void to_dyn_struct(dyn_struct& to, char const* const& from) noexcept {
	dyn_struct::variant v;
	v = std::string(from);
	to.value = std::move(v);
	to.type_hash = typeid(std::string).hash_code();
}

std::string format(const dyn_struct& s, std::string_view indent) noexcept {
	std::string result;
	std::visit([&](auto v) noexcept {
		using type = std::decay_t<decltype(v)>;

		if constexpr (std::is_same_v<type, dyn_struct::integer_t>) {
			result = std::to_string(v);
		}
		else if constexpr (std::is_same_v<type, dyn_struct::real_t>) {
			result = std::to_string(v);
		}
		else if constexpr (std::is_same_v<type, dyn_struct::boolean_t>) {
			result = v ? "true" : "false";
		}
		else if constexpr (std::is_same_v<type, dyn_struct::string_t>) {
			result = '"';
			result += v;
			result += '"';
		}
		else if constexpr (std::is_same_v<type, dyn_struct::array_t>) {
			result += '[';
			for (size_t i = 0; i < v.size(); ++i) {
				result += ' ';
				result += format(*v[i], indent);
				result += ',';
			}

			// We replace the trailling comma by a space
			// [ 0, 1, ..., x,] => [ 0, 1, ..., x ].
			if (!v.empty()) {
				result.back() = ' ';
			}
			result += ']';
		}
		else if constexpr (std::is_same_v<type, dyn_struct::structure_t>) {
			if (v.empty()) {
				result = "{}";
				return;
			}

			result += "{\n";
			for (auto&[key, value] : v) {
				result += indent;
				result += key;
				result += ": ";

				std::string to_indent = format(*value, indent);
				for (size_t i = 0; i < to_indent.size(); ++i) {
					if (to_indent[i] != '\n') continue;
					to_indent.insert(
						std::begin(to_indent) + i + 1, std::begin(indent), std::end(indent)
					);
				}

				result += std::move(to_indent);

				result += "\n";
			}
			result += "}";

		}
		else {
			std::abort();
		}

	}, s.value);

	return result;
}

std::string format_to_json(const dyn_struct& s, std::string_view indent) noexcept {
	std::string result;
	std::visit([&](auto v) noexcept {
		using type = std::decay_t<decltype(v)>;

		if constexpr (std::is_same_v<type, dyn_struct::integer_t>) {
			result = std::to_string(v);
		}
		else if constexpr (std::is_same_v<type, dyn_struct::real_t>) {
			result = std::to_string(v);
		}
		else if constexpr (std::is_same_v<type, dyn_struct::boolean_t>) {
			result = v ? "true" : "false";
		}
		else if constexpr (std::is_same_v<type, dyn_struct::string_t>) {
			result = '"';
			result += v;
			result += '"';
		}
		else if constexpr (std::is_same_v<type, dyn_struct::array_t>) {
			result += '[';
			for (size_t i = 0; i < v.size(); ++i) {
				result += ' ';
				result += format_to_json(*v[i], indent);
				result += ',';
			}

			// We replace the trailling comma by a space
			// [ 0, 1, ..., x,] => [ 0, 1, ..., x ].
			if (!v.empty()) {
				result.back() = ' ';
			}
			result += ']';
		}
		else if constexpr (std::is_same_v<type, dyn_struct::structure_t>) {
			if (v.empty()) {
				result = "{}";
				return;
			}

			result += "{";
			for (auto&[key, value] : v) {
				result += "\n";
				result += indent;
				result += '"';
				result += key;
				result += '"';
				result += ": ";

				std::string to_indent = format_to_json(*value, indent);
				for (size_t i = 0; i < to_indent.size(); ++i) {
					if (to_indent[i] != '\n') continue;
					to_indent.insert(
						std::begin(to_indent) + i + 1, std::begin(indent), std::end(indent)
					);
				}

				result += std::move(to_indent);
				result += ',';
			}
			result.pop_back();
			result += "\n}";

		}
		else {
			std::abort();
		}

	}, s.value);

	return result;
}
std::string format_to_json(const dyn_struct& s, size_t space_indent) noexcept {
	std::string str(space_indent, ' ');
	return format_to_json(s, str);
}
std::string format(const dyn_struct& s, size_t space_indent) noexcept {
	std::string indent(space_indent, ' ');
	return format(s, indent);
}
std::string format_to_json(const dyn_struct& s) noexcept {
	return format_to_json(s, "\t");
}
std::string format(const dyn_struct& s) noexcept {
	return format(s, "\t");
}

std::optional<dyn_struct> load_from_json_file(const std::filesystem::path& file) noexcept {
	enum class token_type {
		OPEN_CURLY,
		CLOSE_CURLY,
		OPEN_BRACKET,
		CLOSE_BRACKET,
		COMMA,
		COLON,
		STRING,
		NULL_JSON,
		TRUE,
		FALSE,
		NUMBER,
		UNKNOWN
	};
	struct token {
		token_type type;
		size_t size;
		size_t idx;
	};

	auto is_whitespace = [](char c) {
		return
			c == '\x0009' ||
			c == '\x000a' ||
			c == '\x000d' ||
			c == '\x0020';
	};
	auto is_part_of_number = [](char c) {
		return
			c == '-' ||
			c == '+' ||
			c == 'e' ||
			c == 'E' ||
			c == '.' ||
			(c >= '0' && c <= '9');
	};

	auto get_string_from_escape_seq = [](const std::vector<char>& seq, size_t idx)
		-> std::optional<std::pair<std::string, size_t>> {
		switch (seq[idx])
		{
		case '"':	return std::pair{ "\"", idx + 1 };
		case '\\':	return std::pair{ "\\", idx + 1 };
		case '/':	return std::pair{ "/", idx + 1 };
		case 'b':	return std::pair{ "\b", idx + 1 };
		case 'f':	return std::pair{ "\f", idx + 1 };
		case 'n':	return std::pair{ "\n", idx + 1 };
		case 'r':	return std::pair{ "\r", idx + 1 };
		case 't':	return std::pair{ "\t", idx + 1 };
		case 'u': {
			if (idx + 5 >= seq.size()) return std::nullopt;

			std::string str(std::begin(seq) + idx + 1, std::begin(seq) + idx + 5);
			size_t unicode_point = std::stoi(str, nullptr, 16);

			return std::pair{ codepoint_to_utf8(unicode_point), idx + 5 };
		}
		default:
			return std::nullopt;
		}
	};

	auto advance_token = [=](const std::vector<char>& str, size_t idx)
		-> std::pair<token, size_t>
	{
		if (idx >= str.size()) return { { token_type::UNKNOWN }, idx + 1 };

		while (idx < str.size() && is_whitespace(str[idx])) idx++;
		if (idx >= str.size()) return { { token_type::UNKNOWN }, idx };

		token new_token;
		switch (str[idx])
		{
		case '{':
			return { { token_type::OPEN_CURLY, 1, idx }, idx + 1 };
		case '}':
			return { { token_type::CLOSE_CURLY, 1, idx }, idx + 1 };
		case '[':
			return { { token_type::OPEN_BRACKET, 1, idx }, idx + 1 };
		case ']':
			return { { token_type::CLOSE_BRACKET, 1, idx }, idx + 1 };
		case ',':
			return { { token_type::COMMA, 1, idx }, idx + 1 };
		case ':':
			return { { token_type::COLON, 1, idx }, idx + 1 };
		case '"':
			new_token.type = token_type::STRING;
			idx++;
			new_token.idx = idx;
			while (idx + 1 < str.size() && str[idx] != '"') {
				if (str[idx] == '\\') {
					idx++;
					if (idx >= str.size()) {
						return { { token_type::UNKNOWN }, idx };
					}
				}
				idx++;
			}
			new_token.size = idx - new_token.idx;
			return { new_token, idx + 1 };
		case '-': case '0': case '1': case '2': case '3': case '4': case '5': case '6':
		case '7': case '8': case '9':
			new_token.type = token_type::NUMBER;
			new_token.idx = idx;

			while (idx + 1 < str.size() && is_part_of_number(str[idx])) idx++;

			new_token.size = idx - new_token.idx;
			return { new_token, idx };
		case 'f': case 't': case 'n':
			if (idx + 4 < str.size() && memcmp("true", &str[idx], 4) == 0)
				return { {token_type::TRUE, 4, idx }, idx + 4 };
			if (idx + 4 < str.size() && memcmp("null", &str[idx], 4) == 0)
				return { {token_type::NULL_JSON, 4, idx }, idx + 4 };
			if (idx + 5 < str.size() && memcmp("false", &str[idx], 5) == 0)
				return { {token_type::FALSE, 5, idx }, idx + 5 };
			return { { token_type::UNKNOWN }, idx + 1 };
		default:
			return { { token_type::UNKNOWN }, idx + 1 };
		}
	};

	auto bytes_opt = read_whole_file(file);
	if (!bytes_opt) return std::nullopt;

	auto bytes = std::move(*bytes_opt);

	std::vector<token> tokens;
	auto [current_token, current_idx] = advance_token(bytes, 0);
	while (current_token.type != token_type::UNKNOWN) {
		tokens.push_back(current_token);
		std::tie(current_token, current_idx) = advance_token(bytes, current_idx);
	}

	std::function<
		std::optional<std::pair<dyn_struct, size_t>>(const std::vector<token>&, size_t)
	> construct_value;
	std::function<
		std::optional<std::pair<dyn_struct, size_t>>(const std::vector<token>&, size_t)
	> construct_object;
	std::function<
		std::optional<std::pair<dyn_struct, size_t>>(const std::vector<token>&, size_t)
	> construct_array;

	construct_value =
		[&](const std::vector<token>& tokens, size_t idx)
		-> std::optional<std::pair<dyn_struct, size_t>>
	{
		dyn_struct result;
		switch (tokens[idx].type) {
		case token_type::STRING: {
			std::string str(
				std::begin(bytes) + tokens[idx].idx,
				std::begin(bytes) + tokens[idx].idx + tokens[idx].size
			);
			result = str;
			return std::pair{ result, idx + 1 };
		}
		case token_type::NUMBER: {
			std::string str(
				std::begin(bytes) + tokens[idx].idx,
				std::begin(bytes) + tokens[idx].idx + tokens[idx].size
			);
			auto number = std::atof(str.data());
			if (fmod(number, 1.0) == 0) result.value = dyn_struct::integer_t(number);
			else result.value = dyn_struct::real_t(number);

			return std::pair{ result, idx + 1 };
		}
		case token_type::OPEN_CURLY:
			return construct_object(tokens, idx);
		case token_type::OPEN_BRACKET:
			return construct_array(tokens, idx);
		case token_type::TRUE:
			result.value = true;
			return std::pair{ result, idx + 1 };
		case token_type::FALSE:
			result.value = false;
			return std::pair{ result, idx + 1 };
		case token_type::NULL_JSON:
			result.value = nullptr;
			return std::pair{ result, idx + 1 };
		default:
			return std::nullopt;
		}
	};
	
	construct_object =
		[&](const std::vector<token>& tokens, size_t idx)
		-> std::optional<std::pair<dyn_struct, size_t>>
	{
		if (tokens[idx].type != token_type::OPEN_CURLY) return std::nullopt;
		idx++;
		dyn_struct result;
		result.value = dyn_struct::structure_t{};

		if (idx >= tokens.size()) return std::nullopt;

		while (tokens[idx].type != token_type::CLOSE_CURLY) {
			if (tokens[idx].type != token_type::STRING) return std::nullopt;
			std::string key(
				std::begin(bytes) + tokens[idx].idx,
				std::begin(bytes) + tokens[idx].idx + tokens[idx].size
			);
			idx++;

			if (idx >= tokens.size()) return std::nullopt;
			if (tokens[idx].type != token_type::COLON) return std::nullopt;
			idx++;

			auto value_opt = construct_value(tokens, idx);
			if (!value_opt) return std::nullopt;

			dyn_struct value;
			std::tie(value, idx) = *value_opt;
			set(key, value, result);

			if (idx >= tokens.size()) return std::nullopt;
			if (tokens[idx].type == token_type::COMMA) {
				idx++;
				if (idx >= tokens.size()) return std::nullopt;
			}
		}
		return std::pair{ result, idx + 1 };
	};

	construct_array =
		[&](const std::vector<token>& tokens, size_t idx)
		-> std::optional<std::pair<dyn_struct, size_t>>
	{
		if (tokens[idx].type != token_type::OPEN_BRACKET) return std::nullopt;
		idx++;
		dyn_struct result;
		result.value = dyn_struct::array_t{};

		if (idx >= tokens.size()) return std::nullopt;

		while (tokens[idx].type != token_type::CLOSE_BRACKET) {
			auto value_opt = construct_value(tokens, idx);
			if (!value_opt) return std::nullopt;

			dyn_struct value;
			std::tie(value, idx) = *value_opt;
			result.push_back(value);

			if (idx + 1 >= tokens.size()) return std::nullopt;
			if (tokens[idx].type == token_type::COMMA) idx++;
		}
		return std::pair{ result, idx + 1 };
	};


	if (
		tokens.front().type != token_type::OPEN_CURLY ||
		tokens.back().type != token_type::CLOSE_CURLY
	) return std::nullopt;

	auto result = construct_object(tokens, 0);
	if (!result) return std::nullopt;
	return result->first;
}

size_t
save_to_json_file(const dyn_struct& to_save, const std::filesystem::path& path) noexcept {
	if (!std::holds_alternative<dyn_struct::structure_t>(to_save.value))
		return dyn_struct_error::NOT_AN_OBJECT;

	std::string to_write = format_to_json(to_save);

	return overwrite_file(path, to_write);
}
