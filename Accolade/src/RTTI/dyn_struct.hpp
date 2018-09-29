#pragma once
/*
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

struct dyn_struct {

	using integer_t = long long int;
	using real_t = long double;
	using string_t = std::string;
	using boolean_t = bool;
	using structure_t = std::unique_ptr<dyn_struct>;
	using array_t = std::vector<structure_t>;

	enum Kind : char {
		integer,
		real,
		string,
		boolean,
		structure,
		array,
		count
	} kind;

	std::unordered_map<string_t, std::pair<Kind, std::unique_ptr<void>>> values;

	dyn_struct() = default;
	dyn_struct(const dyn_struct& copy) noexcept;
	dyn_struct(dyn_struct&&) = default;

	dyn_struct& operator=(const dyn_struct& copy) noexcept;
	dyn_struct& operator=(dyn_struct&&) = default;

};


*/