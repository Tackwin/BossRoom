#include "Graphics/VAO.hpp"

VAO VAO::create_quad(const Vector2f& size) {
	float vertices[] = {
		+size.x * 0.5f, +size.y * 0.5f, 0.0f, 1.0f, 1.0f,
		+size.x * 0.5f, -size.y * 0.5f, 0.0f, 1.0f, 0.0f,
		-size.x * 0.5f, -size.y * 0.5f, 0.0f, 0.0f, 0.0f,
		-size.x * 0.5f, +size.y * 0.5f, 0.0f, 0.0f, 1.0f
	};

	u32 indices[] = {
		0, 1, 3,
		1, 2, 3
	};

	VAO r;

	r.set_element_data(indices, 6u);
	r.set_vertex_data(vertices, 20u);
	r.set_vertex_attrib(0u, 3u, false, 5u, 0u);
	r.set_vertex_attrib(1u, 2u, false, 5u, 3u);
	r.enable_vertex_attrib(0u);
	r.enable_vertex_attrib(1u);

	return r;
}


VAO::VAO() {
	glGenVertexArrays(1, &_info.vaoId);
	bind();

	glGenBuffers(1, &_info.eboId);
	glGenBuffers(1, &_info.vboId);
}


VAO::VAO(const VAO&& that) : _info(that._info) {

}
VAO& VAO::operator=(const VAO&& that) {
	_info = that._info;
	return *this;
}


void VAO::set_element_data(u32* indices, u32 size, i32 usage) const {
	bind();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _info.eboId);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, 
		sizeof(u32) * size, 
		indices,
		usage
	);
}

void VAO::set_vertex_data(float* vertices, u32 size, i32 usage) const {
	bind();

	glBindBuffer(GL_ARRAY_BUFFER, _info.vboId);
	glBufferData(
		GL_ARRAY_BUFFER,
		sizeof(float) * size,
		vertices,
		usage
	);
}

void VAO::set_vertex_attrib(
	u32 index, u32 size, bool norm, u32 stride, u32 offset
) const {
	bind();

	glVertexAttribPointer(
		index, 
		size, 
		GL_FLOAT, 
		norm ? GL_TRUE: GL_FALSE, 
		stride * sizeof(float), 
		(void*)(offset * sizeof(float))
	);
}
void VAO::enable_vertex_attrib(u32 index) const {
	glBindVertexArray(_info.vaoId);

	glEnableVertexAttribArray(index);
}

void VAO::bind() const {
	glBindVertexArray(_info.vaoId);
}

void VAO::render(u32 size, i32 mode) const {
	glDrawElements(mode, size, GL_UNSIGNED_INT, (void*)0);
}