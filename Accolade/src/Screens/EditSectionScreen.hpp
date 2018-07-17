#pragma once

#include "Screen.hpp"

#include <SFML/Graphics.hpp>

class LevelScreen;
class EditSectionScreen : public Screen {
public:
	EditSectionScreen(std::weak_ptr<LevelScreen> levelScreen);

	void update(double dt);
	void render(sf::RenderTarget& target);

	void onEnter();
	void onExit();
private:
	std::weak_ptr<LevelScreen> _levelScreen;
};