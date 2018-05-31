#pragma once

#include <unordered_map>

#include "MemoryManager.hpp"
#include "Graphics/Texture.hpp"

class AssetManager {
public:

	static AssetManager& I() noexcept;

	bool add_texture(const std::string& key, const std::string& path);
	Texture& get_texture(const std::string& key);
	bool find_texture(const std::string& key);

private:

	using TextureMap = std::unordered_map<
		std::string,
		Texture,
		std::hash<std::string>,
		std::equal_to<std::string>,
		MM::Allocator<std::pair<const std::string, Texture>>
	>;
	TextureMap _textures;
};

using AM = AssetManager;