#include <Graphics/GUI/Widget.hpp>

#include <queue>

#include <Managers/InputsManager.hpp>

const Widget::Callback::type Widget::Callback::ZERO = []() { return false; };

Widget::Widget() {
}

Widget::Widget(Widget* const parent) :
	_parent(parent) 
{
}

Widget::~Widget() {
}

void Widget::emancipate(){
	if (!_parent)
		return;

	if (_parent->haveChild(this))
		_parent->denyChild(this);

	_parent = nullptr;
}
void Widget::denyChild(Widget* const child) {
	if (!haveChild(child))
		return;

	const auto& it = std::find_if(_childs.begin(), _childs.end(), 
		[child](const std::pair<int32_t, Widget*>& A) -> bool {
			return A.second == child;
		}
	);

	Widget* c = it->second;
	_childs.erase(it);
	c->emancipate();
}
void Widget::addChild(Widget* const child, int32_t z) {
	if (!child) return;

	if (child == _parent)
		return;

	if (!haveChild(child)) {
		_childs.push_back({ z, child });
		std::sort(_childs.begin(), _childs.end(), 
			[](const std::pair<int32_t, Widget*>& A, const std::pair<int32_t, Widget*>& B) -> bool {
				return A.first < B.first;
			}
		);
	}
	if (child->getParent() != this) {
		child->setParent(this);
	}
}
bool Widget::haveChild(const Widget* const child) {
	if (!child) return false;

	return std::find_if(_childs.begin(), _childs.end(), [child](const auto& A) -> bool { return A.second == child; }) != _childs.end();
}
void Widget::setParent(Widget* const parent, int32_t z) {
	if (!parent) return;

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


Vector2f Widget::getSize() const {
	return _size;
}
const Vector2f& Widget::getOrigin() const {
	return _origin;
}
const Vector2f& Widget::getPosition() const {
	return _pos;
}
Vector2f Widget::getGlobalPosition() const {
	return _pos + (_parent ? _parent->getGlobalPosition() : Vector2f{0, 0});
}
Rectangle2f Widget::getGlobalBoundingBox() const {
	return {
		getGlobalPosition() - Vector2f(_origin.x * _size.x, _origin.y * _size.y),
		getSize()
	};
}
bool Widget::isVisible() const {
	return _visible;
}


void Widget::setSize(const Vector2f& size) {
	_size = size;
}
void Widget::setPosition(const Vector2f& pos) {
	_pos = pos;
}
void Widget::setOrigin(const Vector2f& origin) {
	_origin = origin;
}
void Widget::setOriginAbs(const Vector2f& origin) {
	_origin.x = origin.x / getSize().x;
	_origin.y = origin.y / getSize().y;
}
void Widget::setVisible(bool visible) {
	_visible = visible;
}


const std::vector<std::pair<int32_t, Widget*>> Widget::getChilds() {
	return _childs;
}

void Widget::render(sf::RenderTarget&) {}
void Widget::propagateRender(sf::RenderTarget& target) {
	std::queue<Widget*> open;
	open.push(this);

	while (!open.empty()) {
		auto w = open.front();
		open.pop();
		w->render(target);

		auto child = w->getChilds();
		
		if (!w->isVisible()) 
			continue;

		for (auto& [_, c] : child) {
			open.push(c);
		}
	}
}

std::array<bool, 9> Widget::input(const std::array<bool, 9>& mask) {
	std::array<bool, 9> result = { false };

	if (!_visible) return result;

	if (getGlobalBoundingBox().in(InputsManager::getMouseScreenPos())) {
		if (InputsManager::isMouseJustPressed(sf::Mouse::Left) && !mask[0]) {
			if (_onClick.began) 
				result[0] = _onClick.began();
		}
		if (InputsManager::isMousePressed(sf::Mouse::Left) && !mask[1]) {
			if (_onClick.going)
				result[1] = _onClick.going();
		}
		if (InputsManager::isMouseJustReleased(sf::Mouse::Left) && !mask[2]) {
			if (_onClick.ended)
				result[2] = _onClick.ended();
		}
	}

	return result;
}

void Widget::propagateInput() {
	struct Node {
		Node() {};
		Node(Widget* w) : widget(w) {};
		Node(std::array<bool, 9> mask, Widget* w) : mask(mask), widget(w) {};

		std::array<bool, 9> mask = { false };
		Widget* widget = nullptr;
	};

	std::queue<Node> open;

	constexpr std::array<bool, 9> allFalse = { false, false, false, false, false, false, false, false, false, };
	constexpr std::array<bool, 9> allTrue  = { true, true, true, true, true, true, true, true, true  };

	open.push({ allFalse, this });
	while (!open.empty()) {
		auto w = open.front();
		open.pop();
		auto mask = w.widget->input(w.mask);
		if (mask == allTrue) continue;

		auto child = w.widget->getChilds();
		for (auto&[_, c] : child) {
			open.push(Node(mask, c));
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
