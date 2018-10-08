#pragma once
#include "Widget.hpp"

#include <functional>
#include <unordered_map>

#include <SFML/Graphics.hpp>

#include "3rd/json.hpp"
#include "Utils/UUID.hpp"

class Panel;
class BoolPicker : public Widget {
public:
	using ChangeCallback = std::function<void(bool)>;

	static constexpr auto NAME = "BoolPicker";

	BoolPicker(const nlohmann::json& json) noexcept;

	virtual void render(sf::RenderTarget& target) noexcept;

	UUID listenChange(ChangeCallback&& f) noexcept;
	void stopListeningChange(UUID id) noexcept;

	void setX(bool x) noexcept;
	bool getX() const noexcept;

	virtual void setSize(const Vector2f& size) noexcept override;
private:
	bool onClickEnded() noexcept;

	Panel* panel{ nullptr };

	bool x{ false };

	std::unordered_map<UUID, ChangeCallback> listeners;
};