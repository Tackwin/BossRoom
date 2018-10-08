#include "Label.hpp"

#include "./../../Managers/AssetsManager.hpp"

Label::Label() :
	Widget() {
	computeSize();
}
Label::Label(nlohmann::json json) noexcept : Widget(json) {
	if (auto it = json.find("font"); it != json.end()) {
		setFont(*it);
	}
	if (auto it = json.find("charSize"); it != json.end()) {
		setCharSize(*it);
	}
	if (auto it = json.find("text"); it != json.end()) {
		setStdString(*it);
	}
	if (auto it = json.find("textColor"); it != json.end()) {
		setTextColor(Vector4f::loadJson(*it));
	}
	computeSize();
	_text.setOrigin({
		_origin.x * _text.getLocalBounds().width,
		_origin.y * _text.getLocalBounds().height
	});
	_text.setPosition(getGlobalPosition());
}

void Label::setFont(const std::string& fontKey) {
	_text.setFont(AssetsManager::getFont(fontKey));
	computeSize();
}
void Label::setStdString(const std::string& message) {
	_text.setString(message);
	computeSize();
}
void Label::setSfString(const sf::String& message) {
	_text.setString(message);
	computeSize();
}
void Label::setCharSize(const u32& size) {
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
		_origin.x * _text.getLocalBounds().width,
		_origin.y * _text.getLocalBounds().height
	});
	_text.setPosition(getGlobalPosition());
	// _size is kinda ignored right now

	target.draw(_text);
}

std::string Label::getString() const {
	return _text.getString();
}

const sf::Text& Label::getText() const {
	return _text;
}

sf::Text& Label::getText() {
	return _text;
}
