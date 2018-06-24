#include "Graphics/FrameBuffer.hpp"

FrameBuffer::FrameBuffer(Vector2u size) {
	glGenFramebuffers(1, &_info.id);
	bind();

	_info.color_texture.bind();
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGB, size.x , size.y, 0,
		GL_RGB, GL_UNSIGNED_BYTE, nullptr
	);
	_info.color_texture.set_parameteri(GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	_info.color_texture.set_parameteri(GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, 
		GL_COLOR_ATTACHMENT0, 
		GL_TEXTURE_2D, 
		_info.color_texture.get_texture_id(), 
		0
	);

	glGenRenderbuffers(1, &_info.rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, _info.rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, size.x, size.y);
	glFramebufferRenderbuffer(
		GL_RENDERBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _info.rbo
	);

	_info.quad = VAO::create_quad({ (float)size.y, (float)size.x });

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		throw std::runtime_error("Couldn't create framebuffer.");
	}
}

void FrameBuffer::bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, _info.id);
	glViewport(0, 0, _info.size.x, _info.size.y);
}

void FrameBuffer::clear(Vector4f color) const {
	glClearColor(COLOR_UNROLL(color));
	clear_buffer(GL_COLOR_BUFFER_BIT);
}
void FrameBuffer::clear_buffer(u32 bitfield) const {
	glClear(bitfield);
}

void FrameBuffer::render_texture() const {
	_info.quad.bind();
	_info.color_texture.bind();
	_info.quad.render(6);
}

