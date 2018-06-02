#pragma once
#include "Common.hpp"

class RenderTarget {
	struct RenderTargetInfo {
		u32 fb;
	};
	
public:
	RenderTarget() noexcept;

	u32 set_render_target(u32 target) const noexcept;

private:
	RenderTargetInfo _info;
};