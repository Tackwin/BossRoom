#include "Panel.hpp"

Panel::Panel() :
	Widget() {
	computeSize();
}


sf::Sprite& Panel::getSprite() {
	return _backSprite;
}

void Panel::setSprite(const sf::Sprite& sprite) {
	_backSprite = sprite;
	computeSize();
}

void Panel::computeSize() {
	_size.x = _backSprite.getGlobalBounds().width;
	_size.y = _backSprite.getGlobalBounds().height;
}

void Panel::render(sf::RenderTarget& target) {
	Widget::render(target);
	if (!_visible) return;
		
	_backSprite.setScale(
		_size.x / _backSprite.getTextureRect().width,
		_size.y / _backSprite.getTextureRect().height
	);
	_backSprite.setOrigin({
		_origin.x * _backSprite.getTextureRect().width,
		_origin.y * _backSprite.getTextureRect().height
	});

	_backSprite.setPosition(getGlobalPosition());
	target.draw(_backSprite);
}