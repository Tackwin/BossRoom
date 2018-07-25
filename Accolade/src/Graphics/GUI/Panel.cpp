#include "Panel.hpp"
#include "Managers/AssetsManager.hpp"


Panel::Panel(nlohmann::json json) noexcept : Widget(json) {
	if (auto it = json.find("sprite"); it != json.end()) {
		setTexture(*it);
	}
}

sf::Sprite& Panel::getSprite() {
	return _backSprite;
}

void Panel::setTexture(std::string texture) {
	_texture = texture;
	_backSprite.setTexture(AM::getTexture(texture));
	computeSize();
}
std::string Panel::getTexture() const noexcept {
	return _texture;
}

void Panel::computeSize() {
	_size.x = _backSprite.getGlobalBounds().width;
	_size.y = _backSprite.getGlobalBounds().height;
}

float Panel::getSizeRatio() const noexcept {
	return _size.x / _size.y;
}

void Panel::render(sf::RenderTarget& target) {
	Widget::render(target);
	if (!_visible) return;
		
	sf::CircleShape marker{ 2.0f };
	marker.setOrigin(marker.getRadius(), marker.getRadius());
	marker.setPosition(getGlobalPosition());

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
	target.draw(marker);
}