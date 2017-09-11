#pragma once
#include "Widget.hpp"

#include <Graphics/GUI/Panel.hpp>
#include <Graphics/GUI/Label.hpp>

class Button : public Widget {
public:
	Button();

	sf::Sprite& getSprite();
	sf::Sprite& getHoldSprite();

	void setSprite(const sf::Sprite& sprite);
	void setHoldSprite(const sf::Sprite& sprite);

	void computeSize();

	virtual void render(sf::RenderTarget& target);
private:

	Panel _normal;
	Panel _hold;

	Label _label;
};