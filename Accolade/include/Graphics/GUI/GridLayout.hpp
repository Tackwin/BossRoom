#pragma once
#include "Widget.hpp"

class GridLayout : public Widget {
public:
	GridLayout();
	GridLayout(uint32_t row, uint32_t col);

	void setRow(uint32_t row);
	uint32_t getRow();

	void setCol(uint32_t col);
	uint32_t getCol();

	void updatePos();
	void computeSize();

	void setInternalPadding(const Vector2f& padding);
	Vector2f getInternalPadding();

	void setExternalPadding(const Vector2f& padding);
	Vector2f getExternalPadding();

protected:
	uint32_t _row = 0u;
	uint32_t _col = 0u;

	Vector2f _internalPadding = { 0, 0 };
	Vector2f _externalPadding = { 0, 0 };
};

class HorizontalLayout : public GridLayout {
public:
	HorizontalLayout(uint32_t col);
};

class VerticalLayout : public GridLayout {
public:
	VerticalLayout(uint32_t row);
};