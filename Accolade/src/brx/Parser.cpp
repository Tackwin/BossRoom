#include "Parser.hpp"

#include <fstream>
#include <sstream>
#include <cassert>
#include <tuple>

#include "Utils/string_algorithms.hpp"

using namespace brx;

bool start_with_comment(std::istream& stream) noexcept;
std::pair<Key, Value> read_next_key_value(std::istream& stream) noexcept;
Key read_key(std::istream& stream) noexcept;
Value read_value(std::istream& stream) noexcept;

Number read_number(std::istream& stream) noexcept;
String read_string(std::istream& stream) noexcept;
Array read_array(std::istream& stream) noexcept;
Map read_map(std::istream& stream) noexcept;

std::unique_ptr<Brx> Brx::read_from(std::filesystem::path path) noexcept {
	std::ifstream input_file{ path };

	if (!input_file.is_open()) return nullptr;

	auto brx = std::make_unique<Brx>();

	while (!input_file.eof()) {
		char c;
		while (input_file.get(c), str::is_whitespace(c));
		if (input_file.eof()) break;

		input_file.clear();
		input_file.unget();

		if (start_with_comment(input_file)) {
			std::getline(input_file, std::string{});
			continue;
		}

		auto pair = read_next_key_value(input_file);
		brx->properties.emplace(std::move(pair));
	}

	return brx;
}

bool start_with_comment(std::istream& stream) noexcept {
	if (stream.eof()) return false;

	char first = (char)stream.get();
	if (first != '/') {
		stream.clear();
		stream.unget();
		return false;
	}

	char second = (char)stream.get();
	stream.clear();
	stream.unget();
	stream.unget();

	return first == second;
}

std::pair<Key, Value> read_next_key_value(std::istream& stream) noexcept {
	auto key = read_key(stream);

	char c;
	while (stream.get(c), str::is_whitespace(c));

	stream.clear();
	stream.unget();

	auto value = read_value(stream);

	while (stream.get(c), str::is_whitespace(c));

	// we eat the ending ;
	assert(c == ';');

	return { std::move(key), std::move(value) };
}

Key read_key(std::istream& stream) noexcept {
	std::string key;
	std::getline(stream, key, ':');

	return str::trim_whitespace(key);
}

Value read_value(std::istream& stream) noexcept {
	char c;
	while (stream.get(c), str::is_whitespace(c));
	stream.clear();
	stream.unget();

	switch (c) {
	case '.':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return {
			std::make_unique<Number>(read_number(stream)),
			nullptr,
			nullptr,
			nullptr
		};
		break;
	case '"':
		return {
			nullptr,
			std::make_unique<String>(read_string(stream)),
			nullptr,
			nullptr
		};
		break;
	case '[':
		return {
			nullptr,
			nullptr,
			std::make_unique<Array>(read_array(stream)),
			nullptr
		};
		break;
	case '{':
		return {
			nullptr,
			nullptr,
			nullptr,
			std::make_unique<Map>(read_map(stream))
		};
		break;
	}
}

Number read_number(std::istream& stream) noexcept {
	std::vector<int> numbers;

	int point_pos{ -1 };

	for (char c; stream.get(c), !stream.eof();) {
		if (str::is_whitespace(c)) continue;
		if (!str::is_numeric(c) && !(c == '.' && point_pos == -1)) {
			stream.clear();
			stream.unget();
			break;
		}

		if (c == '.') {
			point_pos = numbers.size();
		}
		else {
			numbers.push_back(c - '0');
		}
	}

	if (point_pos == -1) point_pos = numbers.size();

	Number n{ 0.0 };

	for (int i = 0; i < numbers.size(); ++i) {
		n += numbers[i] * std::powl(10, (long double)(point_pos - (i + 1)));
	}

	return n;
}

String read_string(std::istream& stream) noexcept {
	std::string str;

	// We eat the first "
	stream.seekg(1, stream.cur);

	std::string app;
	std::getline(stream, app, '"');
	str += app;
	if (str.empty()) return str;

	while (str.at(str.size() - 1) == '\\') {

		// We need to replace all '\"' by '"'
		// then we continue to get by '"'
		str.pop_back();
		str += '"';

		std::getline(stream, app, '"');
		str += app;

		if (str.empty()) return str;
	}

	return str;
}

Array read_array(std::istream& stream) noexcept {
	//We eat the first [
	stream.seekg(1, stream.cur);

	Array array;

	char c;
	while (stream.get(c), c != ']') {

		stream.clear();
		stream.unget();

		array.items.push_back(std::move(read_value(stream)));

		while (stream.get(c), str::is_whitespace(c) || c == ',') assert(!stream.eof());

		stream.clear();
		stream.unget();
	}
	
	return array;
}

Map read_map(std::istream& stream) noexcept {
	//We eat the first {
	stream.seekg(1, stream.cur);

	Map map;

	char c;
	while (stream.get(c), c != '}') {
		stream.clear();
		stream.unget();

		map.properties.emplace(std::move(read_next_key_value(stream)));

		while (stream.get(c), str::is_whitespace(c) || c == ';') assert(!stream.eof());

		stream.clear();
		stream.unget();
	}

	return map;
}