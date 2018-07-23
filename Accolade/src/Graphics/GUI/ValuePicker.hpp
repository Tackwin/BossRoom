#pragma once

#include "Widget.hpp"
#include "Label.hpp"

struct ValuePickerInfo {
	Vector4f unfocusedColor{ 0.1f, 0.1f, 0.1f, 0.1f };
	Vector4f focusedColor{ 0.5f, 0.5f, 0.5f, 0.5f };
};

class ValuePicker : public Widget {

public:
	static constexpr auto NAME = "ValuePicker";

	ValuePicker(nlohmann::json json = {}) noexcept;

	virtual void render(sf::RenderTarget& target);

	std::string getText() const noexcept;

private:
	std::string _font;
	std::string _defaultText{ "" };
	std::string _inputString;

	int _charSize{ 14 };

	Vector4f _unfocusedColor{ 0.1f, 0.1f, 0.1f, 0.1f };
	Vector4f _focusedColor{ 0.5f, 0.5f, 0.5f, 0.5f };
	Vector4f _inputColor = _unfocusedColor;

	Label _label;
};