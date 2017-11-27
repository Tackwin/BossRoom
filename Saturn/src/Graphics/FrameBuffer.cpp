#include "Graphics/FrameBuffer.hpp"

FrameBuffer::FrameBuffer() {
	glGenFramebuffers(1, &_info.id);
}

void FrameBuffer::bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, _info.id);
}

void FrameBuffer::attach_color() const {
	bind();
	_info.texture.create_rgba_null({ 700, 700 });
	_info.texture.set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	_info.texture.set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, 
		GL_COLOR_ATTACHMENT0, 
		GL_TEXTURE_2D, 
		_info.texture.get_texture_id(), 
		0u
	);
	glBindFramebuffer(GL_FRAMEBUFFER, 0u);
}

void FrameBuffer::attach_depth() const {
	bind();
	_info.texture.create_depth_null({ 700, 700 });
	_info.texture.set_parameteri(GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	_info.texture.set_parameteri(GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_DEPTH_ATTACHMENT,
		GL_TEXTURE_2D,
		_info.texture.get_texture_id(),
		0u
	);
	glBindFramebuffer(GL_FRAMEBUFFER, 0u);
}
