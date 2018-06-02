#pragma once

#include <unordered_map>
#include <optional>

#include "MemoryManager.hpp"

#include "Graphics/Shader.hpp"
#include "Graphics/Texture.hpp"

class AssetManager {
public:

	static AssetManager& I() noexcept;

	bool load_texture(const std::string& key, const std::string& path);
	std::optional<std::string> load_texture(const std::string& path);
	Texture& get_texture(const std::string& key);
	bool has_texture(const std::string& key);

	bool load_shader(const std::string& key, const std::string& path);
	std::optional<std::string> load_shader(const std::string& path);
	Shader& get_shader(const std::string& key);
	bool has_shader(const std::string& key);

private:

	using TextureMap = std::unordered_map<
		std::string,
		Texture,
		std::hash<std::string>,
		std::equal_to<std::string>,
		MM::Allocator<std::pair<const std::string, Texture>>
	>;
	TextureMap _textures;

	using ShaderMap = std::unordered_map<
		std::string,
		Shader,
		std::hash<std::string>,
		std::equal_to<std::string>,
		MM::Allocator<std::pair<const std::string, Shader>>
	>;
	ShaderMap _shaders;
};

using AM = AssetManager;