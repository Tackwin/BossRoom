#pragma once
#include <SFML/Graphics.hpp>

class Screen {
public:
	virtual void onEnter() = 0;
	virtual void onExit() = 0;

	virtual void update(double dt) = 0;
	virtual void render(sf::RenderTarget& target) = 0;
};

