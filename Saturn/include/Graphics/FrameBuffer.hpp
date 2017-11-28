#pragma once

#include <glad/glad.h>

#include "VAO.hpp"

#include "Math/Vector.hpp"

#include "Common.hpp"

struct FrameBufferInfo {
	u32 id = 0u;
	u32 rbo = 0u;
	u32 texture = 0u;

	VAO quad;
};

class FrameBuffer {
public:
	FrameBuffer(u32 width, u32 height);

	void bind() const;

	void clear(Vector4f color = { 0, 0, 0, 1 }) const;
	void clearBuffer(u32 bitfield) const;

	void render() const;
private:
	FrameBufferInfo _info;
};