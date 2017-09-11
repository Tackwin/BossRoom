#include <Graphics/GUI/GridLayout.hpp>

GridLayout::GridLayout() {
}
GridLayout::GridLayout(uint32_t row, uint32_t col) :
	_row(row),
	_col(col)
{
}


void GridLayout::setRow(uint32_t row) {
	_row = row;
	updatePos();
}
uint32_t GridLayout::getRow() {
	return _row;
}

void GridLayout::setCol(uint32_t col) {
	_col = col;
	updatePos();
}
uint32_t GridLayout::getCol() {
	return _col;
}

void GridLayout::updatePos(){
	computeSize();
	Vector2 size = _size;

	for (uint32_t i = 0u; i < _childs.size(); ++i) {
		Vector2 pos;
		pos.x = _externalPadding.x + (i % _col) * (_internalPadding.x + size.x / _col);
		pos.y = _externalPadding.y + (i / _col) * (_internalPadding.y + size.y / _row);

		_childs[i]->setPosition(pos);
	}
}

void GridLayout::computeSize() {
	Vector2 maxSize;
	
	uint32_t n = 0u;
	for (uint32_t i = 0u; i < _row; ++i) {
		for (uint32_t j = 0u; j < _col; ++j) {
			if (n >= _childs.size()) {
				goto breakLoops; //Yeah i know fuck off, it's a good case he's got so few >:(
			}

			Vector2 size = _childs[i * _col + j]->getSize();
			maxSize.x = std::max(maxSize.x, size.x);
			maxSize.y = std::max(maxSize.y, size.y);
			n++;
		}
	}
breakLoops:

	_size = _externalPadding + Vector2((maxSize.x + _internalPadding.x) * _col, (maxSize.y + _internalPadding.y) * _row);
}


void GridLayout::setInternalPadding(const Vector2& padding) {
	_internalPadding = padding;
}
Vector2 GridLayout::getInternalPadding() {
	return _internalPadding;
}

void GridLayout::setExternalPadding(const Vector2& padding) {
	_externalPadding = padding;
}
Vector2 GridLayout::getExternalPadding() {
	return _externalPadding;
}

HorizontalLayout::HorizontalLayout(uint32_t col) :
	GridLayout(1u, col) 
{
}

VerticalLayout::VerticalLayout(uint32_t row) :
	GridLayout(row, 1u)
{
}
