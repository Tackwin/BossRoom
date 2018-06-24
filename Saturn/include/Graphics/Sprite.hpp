#pragma once

#include "Concepts/Delete.hpp"

#include "FrameBuffer.hpp"
#include "Transform.hpp"
#include "Texture.hpp"
#include "VAO.hpp"

struct SpriteInfo {
	Texture* texture;
	Transform transform;
	VAO mesh;
};

class Sprite : NoCopy {
public:
	Sprite();

	Sprite(const Sprite&& that);
	Sprite& operator=(const Sprite&& that);

	void set_texture(const std::string& key);

	void render(const FrameBuffer& target) const;

private:
	SpriteInfo _info;
};