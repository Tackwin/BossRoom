#include "..\include\Utils.hpp"
#include <fstream>

std::vector<char> Utils::readBytes(const std::string& filename) {
	std::ifstream stream(filename.c_str(), std::ios::ate | std::ios::binary);
	if (!stream.is_open()) {
		throw std::runtime_error("Couldn't open file");
	}

	size_t size = (size_t)stream.tellg();
	std::vector<char> data(size);
	stream.seekg(0u);
	stream.read(data.data(), size);
	stream.close();
	return data;
}