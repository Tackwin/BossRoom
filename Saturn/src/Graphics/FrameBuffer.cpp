#include "Graphics/FrameBuffer.hpp"

FrameBuffer::FrameBuffer(u32 width, u32 height) {
	glGenFramebuffers(1, &_info.id);
	bind();

	glGenTextures(1, &_info.texture);

	glBindTexture(GL_TEXTURE_2D, _info.texture);
	glTexImage2D(
		GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr
	);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _info.texture, 0
	);

	glGenRenderbuffers(1, &_info.rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, _info.rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(
		GL_RENDERBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _info.rbo
	);

	_info.quad = std::move(VAO::create_quad({ width, height }));
}

void FrameBuffer::bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, _info.id);
}

void FrameBuffer::clear(Vector4f color) const {
	glClearColor(COLOR_UNROLL(color));
	clearBuffer(GL_COLOR_BUFFER_BIT);
}
void FrameBuffer::clearBuffer(u32 bitfield) const {
	glClear(bitfield);
}

void FrameBuffer::render() const {

}
