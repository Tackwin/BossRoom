#pragma once

#include <memory>

#include "Transform.hpp"
#include "Texture.hpp"
#include "VAO.hpp"


class Sprite {
	struct SpriteInfo {
		std::string textureKey;
		std::string shaderKey;
		Transform transform;
		VAO mesh;
	};

public:
	Sprite();

	void set_shader(const std::string& shader) noexcept;
	void set_texture(const std::string& key) noexcept;
	Transform& get_transform() noexcept;

	void render() const;

private:
	SpriteInfo _info;
};