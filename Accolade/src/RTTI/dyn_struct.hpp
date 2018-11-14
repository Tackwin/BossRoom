#ifndef DYN_STRUCT_HPP
#define DYN_STRUCT_HPP

#include <any>
#include <string>
#include <memory>
#include <vector>
#include <variant>
#include <optional>
#include <filesystem>
#include <unordered_map>

#include "Memory/ValuePtr.hpp"

struct dyn_struct {
	using integer_t = long long int;
	using real_t = long double;
	using string_t = std::string;
	using boolean_t = bool;
	using structure_t = std::unordered_map<std::string, ValuePtr<dyn_struct>>;
	using array_t = std::vector<ValuePtr<dyn_struct>>;
	using null_t = std::nullptr_t;

	using variant =
		std::variant<integer_t, real_t, string_t, boolean_t, structure_t, array_t, null_t>;

	dyn_struct() = default;

	dyn_struct(const dyn_struct&) = default;
	dyn_struct(dyn_struct&&) = default;

	dyn_struct& operator=(const dyn_struct&) = default;
	dyn_struct& operator=(dyn_struct&&) = default;
	template<typename T>
	dyn_struct(std::initializer_list<T> list) noexcept {
		array_t array;

		for (auto& x : list) {
			array.push_back({ new dyn_struct(x) });
		}

		value = array;
	}

	dyn_struct(std::initializer_list<std::pair<std::string, dyn_struct>> list) noexcept;

	template<typename T> dyn_struct(T&& object) noexcept {
		if constexpr (std::is_same_v<std::decay_t<T>, dyn_struct>) {
			*this = std::forward<T>(object);
		}
		else {
			to_dyn_struct(*this, std::forward<T>(object));
		}
	}

	template<typename T> operator T() const noexcept {
		T t;
		from_dyn_struct(*this, t);
		return t;
	}

	ValuePtr<dyn_struct>& operator[](std::string_view str) noexcept;
	ValuePtr<dyn_struct>& operator[](size_t idx) noexcept;

	void push_back(const dyn_struct& v) noexcept;
	void pop_back() noexcept;

	dyn_struct* clone() noexcept;

	size_t type_hash{ typeid(dyn_struct).hash_code() };
	variant value;
};

#define X(x)\
	extern void to_dyn_struct(dyn_struct&, const x&) noexcept;\
	extern void from_dyn_struct(const dyn_struct&, x&) noexcept;

X(dyn_struct::integer_t)
X(dyn_struct::real_t)
X(dyn_struct::string_t)
X(dyn_struct::boolean_t)
X(dyn_struct::array_t)
X(dyn_struct::structure_t)
X(dyn_struct::null_t)
X(int)
X(long)
X(unsigned)
X(long unsigned)
X(long long unsigned)
X(short)
X(unsigned short)
X(float)
X(double)

#undef X

extern dyn_struct&
set(std::string_view str, const dyn_struct& value, dyn_struct& to) noexcept;

extern std::string format(const dyn_struct& s) noexcept;
extern std::string format(const dyn_struct& s, std::string_view indent) noexcept;
extern std::string format(const dyn_struct& s, size_t space_indent) noexcept;
extern std::optional<dyn_struct> load_from_json_file(const std::filesystem::path& file) noexcept;
extern size_t
save_to_file(const dyn_struct& to_save, const std::filesystem::path& file) noexcept;

#endif