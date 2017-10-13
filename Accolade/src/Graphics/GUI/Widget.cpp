#include <Graphics/GUI/Widget.hpp>

#include <queue>

#include <Managers/InputsManager.hpp>

const Widget::Callback::type Widget::Callback::ZERO = []() {};

Widget::Widget() {
}

Widget::Widget(Widget* const parent) :
	_parent(parent) 
{
}

Widget::~Widget() {
}
void Widget::addChild(Widget* const child, int32_t z) {
	if (child == _parent)
		return;

	if (!haveChild(child)) {
		_childs.push_back({ z, child });
		std::sort(_childs.begin(), _childs.end(), [](const auto& A, const auto& B) -> bool {
			return A.first < B.first;
		});
	}
	if (child->getParent() != this) {
		child->setParent(this);
	}
}
bool Widget::haveChild(const Widget* const child) {
	return std::find_if(_childs.begin(), _childs.end(), [child](const auto& A) -> bool { return A.second == child; }) != _childs.end();
}
void Widget::setParent(Widget* const parent, int32_t z) {
	if (haveChild(parent))
		return;

	_parent = parent;
	if (!_parent->haveChild(this)) {
		_parent->addChild(this, z);
	}
}
Widget* const Widget::getParent() {
	return _parent;
}


Vector2 Widget::getSize() const {
	return _size;
}
const Vector2& Widget::getOrigin() const {
	return _origin;
}
const Vector2& Widget::getPosition() const {
	return _pos;
}
Vector2 Widget::getGlobalPosition() const {
	return _pos + (_parent ? _parent->getGlobalPosition() : Vector2::ZERO);
}
bool Widget::isVisible() const {
	return _visible;
}


void Widget::setSize(const Vector2& size) {
	_size = size;
}
void Widget::setPosition(const Vector2& pos) {
	_pos = pos;
}
void Widget::setOrigin(const Vector2& origin) {
	_origin = origin;
}
void Widget::setOriginAbs(const Vector2& origin) {
	_origin.x = origin.x / getSize().x;
	_origin.y = origin.y / getSize().y;
}
void Widget::setVisible(bool visible) {
	_visible = visible;
}

const std::vector<std::pair<int32_t, Widget*>> Widget::getChilds() {
	return _childs;
}

void Widget::render(sf::RenderTarget&) { }
void Widget::propagateRender(sf::RenderTarget& target) {
	std::queue<Widget*> open;
	std::vector<Widget*> close;

	open.push(this);
	while (!open.empty()) {
		auto w = open.front();
		open.pop();
		w->render(target);

		auto child = w->getChilds();
		if (!w->isVisible()) continue;
		for (auto& [_, c] : child) {
			if (std::find(close.begin(), close.end(), c) != close.end()) { // i think i can get rid of this given that my graph is a tree
				continue;
			}

			open.push(c);
			close.push_back(c);
		}
	}
}

void Widget::input() {
	if (!_visible) return;

	if (InputsManager::getMouseScreenPos().isInRec(getGlobalPosition() - Vector2(_origin.x * _size.x, _origin.y * _size.y), _size)) {
		if (InputsManager::isMouseJustPressed(sf::Mouse::Left)) {
			if (_onClick.began) 
				_onClick.began();
		}
		if (InputsManager::isMousePressed(sf::Mouse::Left)) {
			if (_onClick.going)
				_onClick.going();
		}
		if (InputsManager::isMouseJustReleased(sf::Mouse::Left)) {
			if (_onClick.ended)
				_onClick.ended();
		}
	}
}

void Widget::propagateInput() {
	std::queue<Widget*> open;
	std::vector<Widget*> close;

	open.push(this);
	while (!open.empty()) {
		auto w = open.front();
		open.pop();
		w->input();

		auto child = w->getChilds();
		for (auto& [_, c] : child) {
			if (std::find(close.begin(), close.end(), c) != close.end()) { // i find i can get rid of this given that my graph is a tree
				continue;
			}

			open.push(c);
			close.push_back(c);
		}
	}
}

void Widget::setOnHover(const Callback& onHover) {
	_onHover = onHover;
}
void Widget::setOnClick(const Callback& onClick) {
	_onClick = onClick;
}
void Widget::setOnKey(const Callback& onKey) {
	_onKey = onKey;
}
