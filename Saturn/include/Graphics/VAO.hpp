#pragma once

#include <glad/glad.h>

#include "Common.hpp"
#include "Math/Vector.hpp"

struct VAOInfo {
	u32 vaoId = 0u;
	u32 eboId = 0u;
	u32 vboId = 0u;
};

class VAO {
public:

	static void create_quad(VAO& vao, const Vector2f& size);

	VAO();

	VAO(const VAO&) = delete;
	VAO(const VAO&&) = delete;
	VAO& operator=(const VAO&) = delete;
	VAO& operator=(const VAO&&) = delete;

	void set_element_data(
		u32* indices, u32 size, i32 usage = GL_STATIC_DRAW
	) const;
	
	void set_vertex_data(
		float* vertices, u32 size, i32 usage = GL_STATIC_DRAW
	) const;

	void set_vertex_attrib(
		u32 index, u32 size, bool norm, u32 stride, u32 offset
	) const;
	void enable_vertex_attrib(u32 index) const;

	void bind() const;

	void render(u32 size, i32 mode = GL_TRIANGLES) const;
private:

	VAOInfo _vaoInfo;

};