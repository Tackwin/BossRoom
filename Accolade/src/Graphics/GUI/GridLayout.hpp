#pragma once
#include "Widget.hpp"

class GridLayout : public Widget {
public:
	GridLayout();
	GridLayout(u32 row, u32 col);

	void setRow(u32 row);
	u32 getRow();

	void setCol(u32 col);
	u32 getCol();

	void updatePos();
	void computeSize();

	void setInternalPadding(const Vector2f& padding);
	Vector2f getInternalPadding();

	void setExternalPadding(const Vector2f& padding);
	Vector2f getExternalPadding();

protected:
	u32 _row = 0u;
	u32 _col = 0u;

	Vector2f _internalPadding = { 0, 0 };
	Vector2f _externalPadding = { 0, 0 };
};

class HorizontalLayout : public GridLayout {
public:
	HorizontalLayout(u32 col);
};

class VerticalLayout : public GridLayout {
public:
	VerticalLayout(u32 row);
};