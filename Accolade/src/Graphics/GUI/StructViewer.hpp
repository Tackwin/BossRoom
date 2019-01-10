#pragma once
#include "Widget.hpp"

#include "RTTI/dyn_struct.hpp"

struct StructViewerOpts {
	dyn_struct d_struct;
	Vector2f max_size;
	Vector2f pos;

	size_t font_size{ 14 };

	bool draggable{ false };
	bool collapsable{ false };
};

struct StructViewer : public Widget {
	StructViewer(const StructViewerOpts& opts) noexcept;

	dyn_struct d_struct;
};
