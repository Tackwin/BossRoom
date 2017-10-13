#pragma once
#include "Widget.hpp"

#include "SFML/Graphics.hpp"

class Panel : public Widget {
public:

	Panel();

	void setSprite(const sf::Sprite& sprite);
	sf::Sprite& getSprite();

	void computeSize();

	void render(sf::RenderTarget& target);

protected:
	sf::Sprite _backSprite;
};