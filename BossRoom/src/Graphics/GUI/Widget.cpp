#include "../../include/Graphics/GUI/Widget.hpp"

Widget::Widget() {
}

Widget::Widget(Widget* const parent) :
_parent(parent) 
{
}

Widget::~Widget() {
}
void Widget::addChild(Widget* const child) {
	if (child == _parent)
		return;

	if (!haveChild(child)) {
		_childs.push_back(child);
	}
	if (child->getParent() != this) {
		child->setParent(this);
	}
}
bool Widget::haveChild(const Widget* const child) {
	return std::find(_childs.begin(), _childs.end(), child) != _childs.end();
}
void Widget::setParent(Widget* const parent) {
	if (haveChild(parent))
		return;

	_parent = parent;
	if (!_parent->haveChild(this)) {
		_parent->addChild(this);
	}
}
Widget* const Widget::getParent() {
	return _parent;
}


Vector2 Widget::getSize() const {
	return Vector2();
}
const Vector2& Widget::getOrigin() const {
	return _origin;
}
const Vector2& Widget::getPosition() const {
	return _pos;
}

const Vector2& Widget::setPosition(const Vector2& pos) {
	_pos = pos;
	return _pos;
}
const Vector2& Widget::setOrigin(const Vector2& origin) {
	_origin = origin;
	return _origin;
}
const Vector2& Widget::setOriginAbs(const Vector2& origin) {
	_origin.x = origin.x / getSize().x;
	_origin.y = origin.y / getSize().y;
	return _origin;
}

void Widget::render(sf::RenderTarget&) {
}
