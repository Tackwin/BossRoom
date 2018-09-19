#pragma once
#include "Widget.hpp"

#include <unordered_map>

#include "Utils/UUID.hpp"

class Label;
class Panel;

struct ValuePickerInfo {
	Vector4f unfocusedColor{ 0.1f, 0.1f, 0.1f, 0.1f };
	Vector4f focusedColor{ 0.5f, 0.5f, 0.5f, 0.5f };
};

class ValuePicker : public Widget {
public:
	using ChangeCallback = std::function<void(const std::string&)>;

	static constexpr auto NAME = "ValuePicker";

	ValuePicker(nlohmann::json json = {}) noexcept;

	virtual void render(sf::RenderTarget& target);

	std::string getStdString() const noexcept;
	void setStdString(std::string str) noexcept;

	UUID listenChange(ChangeCallback&& f) noexcept;
	void stopListeningChange(UUID id) noexcept;

	void setCursorTexture(std::string_view str) noexcept;

	virtual void setSize(const Vector2f& s) noexcept override;
	virtual void setFocus(bool v) noexcept override;

private:

	bool onClickEnded() noexcept;

	bool onHoverBegan() noexcept;
	bool onHoverGoing() noexcept;
	bool onHoverEnded() noexcept;

	bool onKeyBegan() noexcept;

	void setCursorSpritePos() noexcept;

	static constexpr auto CURSOR_SIZE = 2;

	std::string _font;
	std::string _defaultText{ "" };
	std::string _inputString;

	Panel* cursorSprite{ nullptr };
	size_t cursorPos{ 0 };

	int _charSize{ 14 };

	Vector4f _unfocusedColor{ 0.1f, 0.1f, 0.1f, 0.1f };
	Vector4f _focusedColor{ 0.5f, 0.5f, 0.5f, 0.5f };
	Vector4f _inputColor = _unfocusedColor;

	Label* _label;

	Vector2f scroll{ 0.f, 0.f };

	std::unordered_map<UUID, ChangeCallback> changeListeners;
};