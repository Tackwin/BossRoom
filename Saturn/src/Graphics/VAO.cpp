#include "Graphics/VAO.hpp"


VAO::VAO() {
	glGenVertexArrays(1, &_vaoInfo.vaoId);
	bind();

	glGenBuffers(1, &_vaoInfo.eboId);
	glGenBuffers(1, &_vaoInfo.vboId);
}

void VAO::set_element_data(u32* indices, u32 size, i32 usage) const {
	bind();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _vaoInfo.eboId);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, 
		sizeof(u32) * size, 
		indices,
		usage
	);
}

void VAO::set_vertex_data(float* vertices, u32 size, i32 usage) const {
	bind();

	glBindBuffer(GL_ARRAY_BUFFER, _vaoInfo.vboId);
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
	glBindVertexArray(_vaoInfo.vaoId);

	glEnableVertexAttribArray(index);
}

void VAO::bind() const {
	glBindVertexArray(_vaoInfo.vaoId);
}

void VAO::render(u32 size, i32 mode) const {
	glDrawElements(mode, size, GL_UNSIGNED_INT, (void*)0);
}