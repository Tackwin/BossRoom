#pragma once

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

	using variant =
		std::variant<integer_t, real_t, string_t, boolean_t, structure_t, array_t>;

	dyn_struct() = default;

	dyn_struct(dyn_struct&) noexcept = default;
	dyn_struct(dyn_struct&&) noexcept = default;

	//dyn_struct operator=(dyn_struct&) noexcept = default;
	//dyn_struct operator=(dyn_struct&&) noexcept = default;
	template<typename T>
	dyn_struct(std::initializer_list<T> list) noexcept {
		array_t array;

		for (auto& x : list) {
			array.push_back({ new dyn_struct(x) });
		}

		value = array;
	}

	template<typename T> dyn_struct(T&& object) noexcept {
		if constexpr (std::is_same_v<T, dyn_struct>) {
			*this = std::forward<T>(object);
		}
		else {
			extern void to_dyn_struct(dyn_struct&, const T&) noexcept;
			to_dyn_struct(*this, std::forward<T>(object));
		}
	}

	template<typename T> operator T() const noexcept {
		T t;
		extern void from_dyn_struct(const dyn_struct&, T&) noexcept;
		from_dyn_struct(*this, t);
		return t;
	}

	dyn_struct clone() noexcept;

	size_t type_hash{ 0 };
	variant value;
private:

#define X(x)\
	friend void to_dyn_struct(dyn_struct& to, const dyn_struct::x& from) noexcept;\
	friend void from_dyn_struct(const dyn_struct& from, dyn_struct::x& to) noexcept;

	X(integer_t);
	X(real_t);
	X(string_t);
	X(boolean_t);
	X(structure_t);
	X(array_t);
#undef X
#define X(x)\
	friend void to_dyn_struct(dyn_struct& to, const x& from) noexcept;\
	friend void from_dyn_struct(const dyn_struct& from, x& to) noexcept;

	X(int);
	X(long);
	X(unsigned);
	X(long unsigned);
	X(long long unsigned);
	X(short);
	X(unsigned short);
	X(float);
	X(double);

#undef X
};

extern std::string format(const dyn_struct& s) noexcept;
extern std::string format(const dyn_struct& s, std::string_view indent) noexcept;
extern std::string format(const dyn_struct& s, size_t space_indent) noexcept;
extern std::optional<dyn_struct> load_from_file(const std::filesystem::path& path) noexcept;
extern size_t
save_to_file(const dyn_struct& to_save, const std::filesystem::path& path) noexcept;
