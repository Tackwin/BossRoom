#include "Managers/AssetManager.hpp"

#include <string>
#include <iostream>

AssetManager& AssetManager::I() noexcept {
	static AM assetManager;
	return assetManager;
}

bool AssetManager::add_texture(const std::string& key, const std::string& path) {
	std::cout << "Loading " << key << " from " << path << "... ";
	if (auto& it = _textures[key]; it.load_file(path)) {
		it.set_filter(Texture::Filter::Linear);
		it.set_wrap(Texture::Wrap::Repeat);

		std::cout << "success" << std::endl;
		return false;
	} 
	_textures.erase(key);
	std::cout << "failure" << std::endl;
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
