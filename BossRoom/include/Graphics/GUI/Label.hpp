#pragma once
#include <memory>
#include <functional>

#include <SFML/Graphics.hpp>

#include "Widget.hpp"
#include "../../include/Math/Vector2.hpp"

class Label : public Widget {
public:

	Label();

	void setFont(const std::string& fontKey);
	void setString(const std::string& message);
	void setTextColor(const sf::Color& color);
	void setCharSize(const uint32_t& size);

	void computeSize();

	virtual void render(sf::RenderTarget& target);

private:
	std::weak_ptr<const sf::Font> _font;
	sf::Text _text;
};
