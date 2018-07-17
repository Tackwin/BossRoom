#include "EditSectionScreen.hpp"

#include "Managers/InputsManager.hpp"

#include "Game.hpp"

#include "LevelScreen.hpp"

EditSectionScreen::EditSectionScreen(std::weak_ptr<LevelScreen> levelScreen) :
	Screen(), _levelScreen(levelScreen)
{}

void EditSectionScreen::update(double) {
	if (IM::isKeyPressed(sf::Keyboard::LControl) &&
		IM::isKeyJustPressed(sf::Keyboard::E)
	) {
		C::game->exitScreen();
	}
	printf("OUI :D\n");
}

void EditSectionScreen::render(sf::RenderTarget&) {

}

void EditSectionScreen::onEnter() {

}

void EditSectionScreen::onExit() {

}