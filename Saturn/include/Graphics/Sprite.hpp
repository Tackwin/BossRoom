#pragma once

#include "Transform.hpp"
#include "Texture.hpp"
#include "VAO.hpp"

struct SpriteInfo {
	Texture* texture;
	Transform transform;
	VAO mesh;
};

class Sprite {
public:
	Sprite();

	void set_texture(const std::string& key);

	void render() const;

private:
	SpriteInfo _info;
};