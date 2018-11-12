#include "dyn_struct.hpp"

dyn_struct dyn_struct::clone() noexcept {
	return dyn_struct(*this);
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
			result = v;
		}
		else if constexpr (std::is_same_v<type, dyn_struct::array_t>) {
			result += '[';
			for (size_t i = 0; i + 1 < v.size(); ++i) {
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
std::string format(const dyn_struct& s, size_t space_indent) noexcept {
	std::string indent(space_indent, ' ');
	return format(s, indent);
}
std::string format(const dyn_struct& s) noexcept {
	return format(s, "\t");
}
