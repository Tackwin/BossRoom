#pragma once
#include <unordered_map>
#include <filesystem>
#include <variant>
#include <vector>
#include <string>

namespace brx {
	enum ValueKind {
		NUMBER,
		STRING,
		ARRAY,
		MAP,
		COUNT
	};

	using Key = std::string;
	using Number = long double;
	using String = std::string;
	struct Array;
	struct Map;
	class Brx;

	struct Value {
		std::unique_ptr<Number> number;
		std::unique_ptr<String> string;
		std::unique_ptr<Array> array;
		std::unique_ptr<Map> map;
	};

	struct Map {
		std::unordered_map<Key, Value> properties;
	};

	struct Array {
		std::list<Value> items;
	};

	class Brx {
		public:
			static std::unique_ptr<Brx> read_from(std::filesystem::path path) noexcept;
		private:
			std::unordered_map<Key, Value> properties;
	};

};
