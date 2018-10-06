#pragma once
#include "Widget.hpp"

#include <vector>

#include <SFML/Graphics.hpp>

#include "Math/Vector.hpp"
#include "Entity/Eid.hpp"
#include "3rd/json.hpp"

#include "Label.hpp"

class PosPicker : public Widget {
public:
	static constexpr auto NAME = "PosPicker";

	PosPicker(const nlohmann::json& json) noexcept;

	virtual void render(sf::RenderTarget& target) noexcept;

	void setViews(const std::vector<Eid<sf::View>>& views) noexcept;
private:
	bool onFocusGoing() noexcept;

	Label* label{ nullptr };

	Vector2f x;
	std::vector<Eid<sf::View>> views;
};