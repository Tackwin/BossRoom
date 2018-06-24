#pragma once
#include "Common.hpp"

#include "Math/Vector.hpp"

#include "Concept/Move.hpp"
#include "Concept/Copy.hpp"

class RenderTarget : public NonCopyable, public Movable {
	struct RenderTargetInfo {
		u32 fb;
		Vector2u size;
	};
	
public:
	RenderTarget() noexcept;

	void use() const noexcept;

private:
	RenderTargetInfo _info;
};