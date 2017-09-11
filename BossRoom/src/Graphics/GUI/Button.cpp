#include <Graphics/GUI/Button.hpp>

Button::Button() : 
Widget() {

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

	_size.x = std::max({ _normal.getSize().x, _hold.getSize().x, _label.getSize().x });
	_size.y = std::max({ _normal.getSize().y, _hold.getSize().y, _label.getSize().y });
}

void Button::render(sf::RenderTarget& target) {
	Widget::render(target);
	if (!_visible) return;

	_normal.setOrigin(_origin);
	_normal.setSize(_size);
	_normal.setPosition(getGlobalPosition());

	_hold.setOrigin(_origin);
	_hold.setSize(_size);
	_hold.setPosition(getGlobalPosition());

	_label.setOrigin(_origin);
	_label.setSize(_size);
	_label.setPosition(getGlobalPosition());

	_normal.render(target);
	_hold.render(target);
	_label.render(target);
}