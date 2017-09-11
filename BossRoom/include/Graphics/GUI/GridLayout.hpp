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
	Vector2 getSize();

	void setInternalPadding(const Vector2& padding);
	Vector2 getInternalPadding();

	void setExternalPadding(const Vector2& padding);
	Vector2 getExternalPadding();

protected:
	uint32_t _row = 0u;
	uint32_t _col = 0u;

	Vector2 _internalPadding = Vector2::ZERO;
	Vector2 _externalPadding = Vector2::ZERO;
};

class HorizontalLayout : public GridLayout {
public:
	HorizontalLayout(uint32_t col);
};

class VerticalLayout : public GridLayout {
public:
	VerticalLayout(uint32_t row);
};