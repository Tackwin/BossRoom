#pragma once
#include <memory>
#include <functional>

#include <SFML/Graphics.hpp>

#include "Widget.hpp"

class Label : public Widget {
public:

	Label();

	void setFont(const std::string& fontKey);
	void setString(const std::string& message);
	void setTextColor(const sf::Color& color);
	void setCharSize(const u32& size);

	std::string getString() const;

	void computeSize();

	virtual void render(sf::RenderTarget& target);

private:
	std::weak_ptr<const sf::Font> _font;
	sf::Text _text;
};
