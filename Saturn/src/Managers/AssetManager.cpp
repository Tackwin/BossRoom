#include "Managers/AssetManager.hpp"

#include <string>
#include <iostream>

std::unordered_map<std::string, Texture> AM::_textures;

bool AssetManager::add_texture(const std::string& key, const std::string& path) {
	std::cout << "Loading " << key << " from " << path << "... ";
	if (!_textures[std::string(key)].load_file(path)) {
		std::cout << "success :)" << std::endl;
		return false;
	} 
	std::cout << "failure :(" << std::endl;
	return true;
}

Texture& AssetManager::get_texture(const std::string& key) {
	if (const auto& it = _textures.find(key); it == _textures.end()) {
		throw std::runtime_error("Texture not in manager");
	}
	else {
		return it->second;
	}
}

bool AssetManager::find_texture(const std::string& key) {
	return _textures.find(key) != _textures.end();
}
