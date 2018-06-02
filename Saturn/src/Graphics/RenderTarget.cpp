#include "Graphics/RenderTarget.hpp"

#include <glad/glad.h>

RenderTarget::RenderTarget() noexcept {
	glGenFramebuffers(1, &_info.fb);
	glBindFramebuffer(GL_FRAMEBUFFER, _info.fb);
}

u32 RenderTarget::set_render_target(u32 target) const noexcept {
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, target, 0);

	GLenum drawBuffer[] = { GL_COLOR_ATTACHMENT0 };

	glDrawBuffers(1, drawBuffer);
	return glCheckFramebufferStatus(GL_FRAMEBUFFER);
}