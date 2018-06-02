#include "Managers/AssetManager.hpp"

#include <string>
#include <iostream>

#include "Utils/UUID.hpp"

AssetManager& AssetManager::I() noexcept {
	static AM assetManager;
	return assetManager;
}

bool AssetManager::load_texture(const std::string& key, const std::string& path) {
	std::cout << "Loading texture: " << key << " from: " << path << "... ";
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
std::optional<std::string> AssetManager::load_texture(const std::string& path) {
	auto key = (std::string)UUID();
	if (load_texture(key, path)) {
		return key;
	}
	return {};
}

Texture& AssetManager::get_texture(const std::string& key) {
	if (const auto& it = _textures.find(key); it == _textures.end()) {
		throw std::runtime_error("Texture not in manager");
	}
	else {
		return it->second;
	}
}

bool AssetManager::has_texture(const std::string& key) {
	return _textures.find(key) != _textures.end();
}


bool AssetManager::load_shader(const std::string& key, const std::string& path) {
	std::cout << "Loading shader: " << key << " from: " << path << "... ";
	if (auto& it = _shaders[key]; 
		it.load_fragment(path + ".fragment") && 
		it.load_vertex(path + ".vertex") &&
		it.build_shaders()
	) {
		std::cout << "success" << std::endl;
		return false;
	}
	_shaders.erase(key);
	std::cout << "failure" << std::endl;
	return true;
}
std::optional<std::string> AssetManager::load_shader(const std::string& path) {
	auto key = (std::string)UUID();
	if (load_shader(key, path)) {
		return key;
	}
	return {};
}

Shader& AssetManager::get_shader(const std::string& key) {
	if (const auto& it = _shaders.find(key); it == _shaders.end()) {
		throw std::runtime_error("Shader not in manager");
	}
	else {
		return it->second;
	}
}

bool AssetManager::has_shader(const std::string& key) {
	return _shaders.find(key) != _shaders.end();
}
