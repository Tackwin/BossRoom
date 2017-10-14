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
	Vector2f size = _size;

	for (uint32_t i = 0u; i < _childs.size(); ++i) {
		Vector2f pos;
		pos.x = _externalPadding.x + (i % _col) * (_internalPadding.x + size.x / _col);
		pos.y = _externalPadding.y + (i / _col) * (_internalPadding.y + size.y / _row);

		_childs[i].second->setPosition(pos);
	}
}

void GridLayout::computeSize() {
	Vector2f maxSize;
	
	uint32_t n = 0u;
	for (uint32_t i = 0u; i < _row; ++i) {
		for (uint32_t j = 0u; j < _col; ++j) {
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

HorizontalLayout::HorizontalLayout(uint32_t col) :
	GridLayout(1u, col) 
{
}

VerticalLayout::VerticalLayout(uint32_t row) :
	GridLayout(row, 1u)
{
}
