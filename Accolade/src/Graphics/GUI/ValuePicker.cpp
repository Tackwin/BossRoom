#include "ValuePicker.hpp"

#include <queue>

#include "Managers/InputsManager.hpp"

ValuePicker::ValuePicker(nlohmann::json json) noexcept :
	Widget(json)
{
	if (auto it = json.find("unfocusedColor"); it != json.end()) {
		_unfocusedColor = Vector4f::loadJson(*it);
	}
	if (auto it = json.find("focusedColor"); it != json.end()) {
		_focusedColor = Vector4f::loadJson(*it);
	}
	if (auto it = json.find("font"); it != json.end()) {
		_font = it->get<std::string>();
	}
	if (auto it = json.find("charSize"); it != json.end()) {
		_charSize = it->get<int>();
	}
	if (auto it = json.find("text"); it != json.end()) {
		_defaultText = it->get<std::string>();
		_inputString = _defaultText;
	}

	Callback onClick;
	Callback onHover;
	Callback onKey;
	onClick.began = Callback::TRUE;
	onClick.going = Callback::TRUE;
	onClick.ended = std::bind(&ValuePicker::onClickEnded, this);

	onHover.began = std::bind(&ValuePicker::onHoverBegan, this);
	onHover.going = std::bind(&ValuePicker::onHoverGoing, this);
	onHover.ended = std::bind(&ValuePicker::onHoverEnded, this);

	onKey.began = std::bind(&ValuePicker::onKeyBegan, this);
	onKey.going = Callback::TRUE;
	onKey.ended = Callback::TRUE;

	setOnClick(onClick);
	setOnHover(onHover);
	setOnKey(onKey);

	_label = new Label;
	_label->setOrigin({ 0.f, 0.f });
	_label->setPosition({ 0.f, -2.f });
	_label->setCharSize(_charSize);
	_label->setFont(_font);
	addChild(_label);
}

void ValuePicker::render(sf::RenderTarget& target) {
	auto inputBox = getGlobalBoundingBox();

	_label->setSfString(_inputString);

	if (_focused) _inputColor = _focusedColor;

	inputBox.render(target, _inputColor);
}

std::string ValuePicker::getStdString() const noexcept {
	return _inputString;
}

bool ValuePicker::onClickEnded() noexcept {
	_focused = !_focused;
	return true;
}

bool ValuePicker::onHoverBegan() noexcept {
	if (_focused) return true;
	_inputColor = _focusedColor;
	return true;
}
bool ValuePicker::onHoverGoing() noexcept {
	if (IM::getLastScroll() != 0) {
		scroll.x += IM::getLastScroll() * getSize().x / 2.f;
	}
	return true;
}
bool ValuePicker::onHoverEnded() noexcept {
	if (_focused) return true;
	_inputColor = _unfocusedColor;
	return true;
}

bool ValuePicker::onKeyBegan() noexcept {
	if (IM::isKeyJustPressed(sf::Keyboard::BackSpace)) {
		if (_inputString.length() != 0) _inputString.erase(_inputString.length() - 1);
	}
	else if (IM::isKeyJustPressed(sf::Keyboard::Return)) {
		_focused = false;
		_inputColor = _unfocusedColor;
	}
	// Only ascii
	else if (IM::isTextJustEntered() && IM::getTextEntered() < 125) {
		_inputString += (char)IM::getTextEntered();
	}
	return true;
};

void ValuePicker::setStdString(std::string str) noexcept {
	_inputString = str;
}