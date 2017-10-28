#include <Graphics/GUI/GridLayout.hpp>

GridLayout::GridLayout() {
}
GridLayout::GridLayout(u32 row, u32 col) :
	_row(row),
	_col(col)
{
}


void GridLayout::setRow(u32 row) {
	_row = row;
	updatePos();
}
u32 GridLayout::getRow() {
	return _row;
}

void GridLayout::setCol(u32 col) {
	_col = col;
	updatePos();
}
u32 GridLayout::getCol() {
	return _col;
}

void GridLayout::updatePos(){
	computeSize();
	Vector2f size = _size;

	for (u32 i = 0u; i < _childs.size(); ++i) {
		Vector2f pos;
		pos.x = _externalPadding.x + (i % _col) * (_internalPadding.x + size.x / _col);
		pos.y = _externalPadding.y + (i / _col) * (_internalPadding.y + size.y / _row);

		_childs[i].second->setPosition(pos);
	}
}

void GridLayout::computeSize() {
	Vector2f maxSize;
	
	u32 n = 0u;
	for (u32 i = 0u; i < _row; ++i) {
		for (u32 j = 0u; j < _col; ++j) {
			if (n >= _childs.size()) {
				goto breakLoops; //Yeah i know fuck off, it's a good case he's got so few >:(
			}

			Vector2f size = _childs[i * _col + j].second->getSize();
			maxSize.x = std::max(maxSize.x, size.x);
			maxSize.y = std::max(maxSize.y, size.y);
			n++;
		}
	}
breakLoops:

	_size = _externalPadding + Vector2f((maxSize.x + _internalPadding.x) * _col, (maxSize.y + _internalPadding.y) * _row);
}


void GridLayout::setInternalPadding(const Vector2f& padding) {
	_internalPadding = padding;
}
Vector2f GridLayout::getInternalPadding() {
	return _internalPadding;
}

void GridLayout::setExternalPadding(const Vector2f& padding) {
	_externalPadding = padding;
}
Vector2f GridLayout::getExternalPadding() {
	return _externalPadding;
}

HorizontalLayout::HorizontalLayout(u32 col) :
	GridLayout(1u, col) 
{
}

VerticalLayout::VerticalLayout(u32 row) :
	GridLayout(row, 1u)
{
}
