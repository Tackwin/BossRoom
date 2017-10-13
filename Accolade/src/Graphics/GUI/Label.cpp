#include <Graphics/GUI/Label.hpp>

#include <Managers/AssetsManager.hpp>

Label::Label() :
Widget() {
	computeSize();
}

void Label::setFont(const std::string& fontKey) {
	_text.setFont(AssetsManager::getFont(fontKey));
	computeSize();
}
void Label::setString(const std::string& message) {
	_text.setString(message);
	computeSize();
}
void Label::setCharSize(const uint32_t& size) {
	_text.setCharacterSize(size);
	computeSize();
}
void Label::setTextColor(const sf::Color& color) {
	_text.setFillColor(color);
}

void Label::computeSize() {
	_size.x = _text.getGlobalBounds().width;
	_size.y = _text.getGlobalBounds().height;
}

void Label::render(sf::RenderTarget & target) {
	Widget::render(target);
	if (!_visible) return;

	_text.setOrigin({
		_origin.x * _size.x,
		_origin.y * _size.y
	});
	_text.setPosition(getGlobalPosition());
	// _size is kinda ignored right now

	target.draw(_text);
}


