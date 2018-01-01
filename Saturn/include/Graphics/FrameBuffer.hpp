#pragma once

#include <glad/glad.h>

#include "Concepts/Delete.hpp"

#include "VAO.hpp"
#include "Texture.hpp"

#include "Math/Vector.hpp"

#include "Common.hpp"

struct FrameBufferInfo {
	u32 id = 0u;
	u32 rbo = 0u;

	Vector2u size = { 0u, 0u };

	VAO quad;
	Texture color_texture;
};

class FrameBuffer : NoCopy, NoMove {
public:
	FrameBuffer(Vector2u size);

	void bind() const;

	void clear(Vector4f color = { 0, 0, 0, 1 }) const;
	void clear_buffer(u32 bitfield) const;

	void render_texture() const;
private:
	FrameBufferInfo _info;
};