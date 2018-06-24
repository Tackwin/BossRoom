#include "Graphics/RenderTarget/RenderTarget.hpp"

#include <glad/glad.h>

RenderTarget::RenderTarget() noexcept {
	glGenFramebuffers(1, &_info.fb);
	glBindFramebuffer(GL_FRAMEBUFFER, _info.fb);
}
void RenderTarget::use() const noexcept {
	glBindBuffer(GL_FRAMEBUFFER, _info.fb);
	glViewport(0, 0, _info.size.x, _info.size.y);
}