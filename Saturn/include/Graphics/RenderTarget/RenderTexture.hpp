#pragma once

#include "RenderTarget.hpp"

#include "Math/Vector.hpp"

class RenderTexture : public RenderTarget {
	struct RenderTextureInfo {
		Vector2u size;
	};

public:
	RenderTexture(Vector2u size) noexcept;


private:
	RenderTextureInfo _info;
};