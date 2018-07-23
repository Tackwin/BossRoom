#include "ValuePicker.hpp"

#include "Managers/InputsManager.hpp"

ValuePicker::ValuePicker(nlohmann::json json) noexcept : Widget(json) {
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
	onClick.ended = [&]() -> bool {
		_focused = !_focused;
		return true;
	};

	onHover.began = [&]() -> bool {
		_inputColor = _focusedColor;
		return true;
	};
	onHover.going = Callback::TRUE;
	onHover.ended = [&]() -> bool {
		_inputColor = _unfocusedColor;
		return true;
	};

	onKey.began = [&]() -> bool {
		if (IM::isKeyJustPressed(sf::Keyboard::BackSpace)) {
			if (_inputString.length() != 0) _inputString.erase(_inputString.length() - 1);
		}
		// Only ascii
		else if (IM::isTextJustEntered() && IM::getTextEntered() < 125){
			_inputString += (char)IM::getTextEntered();
		}
		return true;
	};
	onKey.going = Callback::TRUE;
	onKey.ended = Callback::TRUE;

	setOnClick(onClick);
	setOnHover(onHover);
	setOnKey(onKey);

	_label.setOrigin({ 0.f, 0.f });
	_label.setPosition({ 0.f, -2.f });
	_label.setCharSize(_charSize);
	_label.setFont(_font);
	addChild(&_label);
}

void ValuePicker::render(sf::RenderTarget& target) {
	Rectangle2f inputBox{ getGlobalPosition(), getSize() };
	_label.setSfString(_inputString);

	if (_focused) _inputColor = _focusedColor;

	inputBox.render(target, _inputColor);
}

std::string ValuePicker::getText() const noexcept {
	return _inputString;
}