#include <Graphics/GUI/Panel.hpp>

sf::Sprite& Panel::getSprite() {
	return _backSprite;
}

void Panel::setSprite(const sf::Sprite& sprite) {
	_backSprite = sprite;
}

Vector2 Panel::getSize() const {
	return {
		_backSprite.getGlobalBounds().width,
		_backSprite.getGlobalBounds().height
	};
}

void Panel::render(sf::RenderTarget& target) {
	_backSprite.setOrigin({
		_origin.x * getSize().x,
		_origin.y * getSize().y
	});

	_backSprite.setPosition(getGlobalPosition());
	target.draw(_backSprite);
}