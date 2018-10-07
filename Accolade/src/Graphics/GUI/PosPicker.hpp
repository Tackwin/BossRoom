#pragma once
#include "Widget.hpp"

#include <vector>

#include <SFML/Graphics.hpp>

#include "Math/Vector.hpp"
#include "Entity/Eid.hpp"
#include "3rd/json.hpp"

#include "Label.hpp"
#include "Panel.hpp"

class PosPicker : public Widget {
public:
	using ChangeCallback = std::function<void(const Vector2f&)>;

	static constexpr auto NAME = "PosPicker";

	PosPicker(const nlohmann::json& json) noexcept;

	virtual void render(sf::RenderTarget& target) noexcept;

	void setViews(const std::vector<Eid<sf::View>>& views) noexcept;

	UUID listenChange(ChangeCallback&& f) noexcept;
	void stopListeningChange(UUID id) noexcept;

	void setX(const Vector2f& x) noexcept;
private:
	bool onClickBegan() noexcept;

	bool onFocusBegan() noexcept;
	bool onFocusGoing() noexcept;
	bool onFocusEnded() noexcept;

	Label* label{ nullptr };
	Panel* panel{ nullptr };

	Vector2f x;
	std::vector<Eid<sf::View>> views;

	std::unordered_map<UUID, ChangeCallback> listeners;
};