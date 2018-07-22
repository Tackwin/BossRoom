#include "Button.hpp"

Button::Button() : 
	Widget() 
{
	//_onClick.began = std::bind(&Button::onClickBegan, this);
	//_onClick.ended = std::bind(&Button::onClickEnded, this);

	_hold.setVisible(false);

	_normal.setParent(this, 0);
	_hold.setParent(this, 1);
	_label.setParent(this, 2);
}

Button::~Button() {
	Widget::~Widget();
}


sf::Sprite& Button::getSprite() {
	return _normal.getSprite();
}

sf::Sprite& Button::getHoldSprite() {
	return _hold.getSprite();
}

void Button::setSprite(const sf::Sprite& sprite) {
	_normal.setSprite(sprite);
	computeSize();
}
void Button::setHoldSprite(const sf::Sprite& sprite) {
	_hold.setSprite(sprite);
	computeSize();
}

void Button::computeSize() {
	_normal.computeSize();
	_hold.computeSize();
	_label.computeSize();

	_size.x = std::max({ 
		_normal.getSize().x, 
		_hold.getSize().x, 
		_label.getSize().x 
	});
	_size.y = std::max({ 
		_normal.getSize().y, 
		_hold.getSize().y, 
		_label.getSize().y 
	});
}

void Button::render(sf::RenderTarget&) {}

void Button::setStdString(const std::string& label) {
	_label.setStdString(label);
}
std::string Button::getString() const {
	return _label.getString();
}

bool Button::onClickBegan() {
	_normal.setVisible(false);
	_hold.setVisible(true);
	return true;
}
bool Button::onClickEnded() {
	_normal.setVisible(true);
	_hold.setVisible(false);
	return true;
}
bool Button::onClickGoing() {
	return false;
}

void Button::setSize(const Vector2f& size) {
	Widget::setSize(size);
	_normal.setSize(size);
	_label.setSize(size);
	_hold.setSize(size);
}
void Button::setOrigin(const Vector2f& origin) {
	Widget::setOrigin(origin);
	_normal.setOrigin(origin);
	_label.setOrigin(origin);
	_hold.setOrigin(origin);
}

Label& Button::getLabel() {
	return _label;
}
