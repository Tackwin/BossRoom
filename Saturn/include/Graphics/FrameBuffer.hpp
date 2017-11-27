#pragma once

#include <glad/glad.h>
#include "Common.hpp"

#include "Texture.hpp"

struct FrameBufferInfo {
	u32 id = 0u;
	u32 rbo = 0u;

	Texture texture;
};

class FrameBuffer {
public:
	FrameBuffer();

	void attach_color() const;
	void attach_depth() const;

	void bind() const;
private:
	FrameBufferInfo _info;
};