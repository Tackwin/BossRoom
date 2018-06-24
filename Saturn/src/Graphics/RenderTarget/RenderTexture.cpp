#include "Graphics/RenderTarget/RenderTexture.hpp"

RenderTexture::RenderTexture(Vector2u size) noexcept :
	RenderTarget()  {
	_info.size = size;
}