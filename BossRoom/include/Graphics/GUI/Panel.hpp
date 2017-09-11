#pragma once
#include "Widget.hpp"

#include <SFML/Graphics.hpp>

class Panel : public Widget {
public:

	void setSprite(const sf::Sprite& sprite);
	sf::Sprite& getSprite();

	Vector2 getSize() const;

	void render(sf::RenderTarget& target);

protected:
	sf::Sprite _backSprite;
};