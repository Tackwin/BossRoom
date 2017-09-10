#include "../../include/Graphics/GUI/Label.hpp"

#include "../../include/Managers/AssetsManager.hpp"

Label::Label() :
Widget() {

}

void Label::setFont(const std::string& fontKey) {
	_text.setFont(AssetsManager::getFont(fontKey));
}
void Label::setString(const std::string& message) {
	_text.setString(message);
}
void Label::setCharSize(const uint32_t& size) {
	_text.setCharacterSize(size);
}
void Label::setTextColor(const sf::Color& color) {
	_text.setFillColor(color);
}


Vector2 Label::getSize() const {
	return { _text.getGlobalBounds().width, _text.getGlobalBounds().height };
}

void Label::render(sf::RenderTarget & target) {
	Widget::render(target);

	_text.setOrigin({
		_origin.x * getSize().x,
		_origin.y * getSize().y
	});

	_text.setPosition(
		(_parent.expired() ? Vector2::ZERO : _parent.lock()->getPosition()) + _pos
	);

	target.draw(_text);
}


