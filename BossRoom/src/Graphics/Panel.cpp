#include "../../include/Graphics/Panel.hpp"

#include "../../include/Managers/AssetsManager.hpp"

Panel::Panel(const std::string& textureKey, const std::string& fontKey) :
	_backgroundSprite(AssetsManager::getTexture(textureKey)),
	_font(std::make_shared<const sf::Font>(AssetsManager::getFont(fontKey))) {
}

void Panel::render(sf::RenderTarget& target) {
	_backgroundSprite.setScale(
		getSize().x / _backgroundSprite.getTextureRect().width,
		getSize().y / _backgroundSprite.getTextureRect().height
	);
	target.draw(_backgroundSprite);
	for (const auto& [_, label] : _labels) { //gosh i love C++17
		sf::CircleShape circle(5.f);
		circle.setOrigin(5.f, 5.f);
		circle.setPosition(label.getPosition());
		target.draw(circle);
		circle.setPosition(Vector2(label.getPosition()) + Vector2(label.getGlobalBounds().width, label.getGlobalBounds().height));
		target.draw(circle);
		target.draw(label);
	}
}

void Panel::addLabel(const std::string& key) {
	sf::Text label("", *_font);
	label.setPosition(_backgroundSprite.getPosition().x + _padding.x, _backgroundSprite.getPosition().y + _padding.y + getLabelsHeight() + _padding.y);
	_labels[key] = label;
}
sf::Text& Panel::getLabel(const std::string& key) {
	return _labels[key];
}

Vector2 Panel::getSize() {
	float maxWidth = 0.f;
	for (const auto&[_, label] : _labels) {
		float w = label.getGlobalBounds().width + 2 * _padding.x;
		maxWidth = maxWidth < w ? w : maxWidth;
	}

	return {
		maxWidth,
		getLabelsHeight() + 2 * _padding.y
	};
}

std::map<std::string, sf::Text>& Panel::getLabels() {
	return _labels;
}

float Panel::getLabelsHeight() {
	float sum = 0.f;
	for (const auto& [_, label] : _labels) {
		sum += label.getGlobalBounds().height;
		sum += _padding.y;
	}
	if(sum > 0.f)
		sum -= _padding.y; // we don't want the last padding to have an effect
	return sum;
}
