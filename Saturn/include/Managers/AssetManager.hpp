#pragma once

#include <string_view>
#include <unordered_map>

#include "Graphics/Texture.hpp"

class AssetManager {
public:

	static bool add_texture(const std::string& key, const std::string& path);
	static Texture& get_texture(const std::string& key);
	static bool find_texture(const std::string& key);

private:

	static std::unordered_map<std::string, Texture> _textures;
};

using AM = AssetManager;