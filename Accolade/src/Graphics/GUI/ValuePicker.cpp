#include "ValuePicker.hpp"

#include <queue>
#include <algorithm>

#include "Managers/AssetsManager.hpp"
#include "Managers/InputsManager.hpp"

#include "Label.hpp"
#include "Panel.hpp"

ValuePicker::ValuePicker(nlohmann::json json) noexcept :
	Widget(json)
{
	cursorSprite = makeChild<Panel>({
		{"visible", false}
	}, 1);

	_label = makeChild<Label>({}, 0);
	_label->setOrigin({ 0.f, 0.f });
	_label->setPosition({ 0.f, -2.f });
	_label->setCharSize(_charSize);
	_label->setFont("consola");

	if (auto it = json.find("unfocusedColor"); it != json.end()) {
		_unfocusedColor = Vector4f::loadJson(*it);
	}
	if (auto it = json.find("focusedColor"); it != json.end()) {
		_focusedColor = Vector4f::loadJson(*it);
	}
	if (auto it = json.find("font"); it != json.end()) {
		_font = it->get<std::string>();
		_label->setFont(_font);
	}
	if (auto it = json.find("charSize"); it != json.end()) {
		_charSize = it->get<int>();
	}
	if (auto it = json.find("text"); it != json.end()) {
		setStdString(it->get<std::string>());
	}
	if (auto it = json.find("cursor"); it != json.end()) {
		setCursorTexture(it->get<std::string>());
	}
	else {
		setCursorTexture("cursor");
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

	cursorPos = getStdString().size();
	setCursorSpritePos();
	cursorSprite->setSize({ (float)CURSOR_SIZE, getSize().y });
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
	cursorSprite->setVisible(_focused);
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
		if (_inputString.length() != 0 && cursorPos > 0) {
			setStdString(getStdString().erase(cursorPos - 1, 1));
			cursorPos = cursorPos - 1;
			setCursorSpritePos();
		}
	}
	else if (IM::isKeyJustPressed(sf::Keyboard::Left)) {
		cursorPos = cursorPos != 0 ? cursorPos - 1 : 0;
		setCursorSpritePos();
	}
	else if (IM::isKeyJustPressed(sf::Keyboard::Right)) {
		cursorPos = std::min(_label->getText().getString().getSize(), cursorPos + 1);
		setCursorSpritePos();
	}
	else if (IM::isKeyJustPressed(sf::Keyboard::Return)) {
		setFocus(false);
		_inputColor = _unfocusedColor;
	}
	// Only ascii
	else if (IM::isTextJustEntered() && IM::getTextEntered() < 125) {
		setStdString(getStdString().insert(cursorPos, 1, (char)IM::getTextEntered()));
		cursorPos++;
		setCursorSpritePos();
	}
	return true;
};

void ValuePicker::setFocus(bool v) noexcept {
	Widget::setFocus(v);
	cursorSprite->setVisible(v);
}

void ValuePicker::setStdString(std::string str) noexcept {
	_inputString = str;
	_label->setStdString(str);
	setCursorSpritePos();
	for (auto& [_, f] : changeListeners)
		f(_inputString);
}

UUID ValuePicker::listenChange(ValuePicker::ChangeCallback&& f) noexcept {
	auto id = UUID{};
	changeListeners.emplace(id, f);
	return id;
}

void ValuePicker::stopListeningChange(UUID id) noexcept {
	assert(changeListeners.count(id) != 0);
	changeListeners.erase(id);
}

void ValuePicker::setCursorTexture(std::string_view str) noexcept {
	cursorSprite->setTexture(std::string{ str });
}

void ValuePicker::setCursorSpritePos() noexcept {
	cursorSprite->setPosition({
		_label->getText().findCharacterPos(cursorPos).x - _label->getText().getPosition().x,
		0
	});
}

void ValuePicker::setSize(const Vector2f& s) noexcept {
	Widget::setSize(s);
	cursorSprite->setSize({ (float)CURSOR_SIZE, getSize().y });
}