#include "RoomSelectorScreen.hpp"

#include "./../Common.hpp"
#include "./../Game.hpp"

#include "./../Managers/AssetsManager.hpp"
#include "./../Managers/InputsManager.hpp"

RoomSelectorScreen::RoomSelectorScreen(u32 n):
	_n(n) {


	_roomNumberText = sf::Text(
		std::string("Room #") + std::to_string(n), 
		AssetsManager::getFont("consola"), 
		36
	);
	_roomNumberText.setPosition(100, 100);

	_infoText = sf::Text(
		std::string("Info: ????"), 
		AssetsManager::getFont("consola"), 
		12
	);
	_infoText.setPosition(100, 300);

	_triesInfoText = sf::Text(
		std::string("Tries 0"), 
		AssetsManager::getFont("consola"), 
		12
	);
	_triesInfoText.setPosition(100, 620);

	_goTextButton = sf::Text(
		std::string("GOOO"), 
		AssetsManager::getFont("consola"), 
		36
	);
	_goTextButton.setFillColor(sf::Color(0, 199, 255));
	_goTextButton.setPosition(500, 340);

	_goSpriteButton = sf::RectangleShape({ 150, 75 });
	_goSpriteButton.setFillColor(sf::Color(255, 38, 38));
	_goSpriteButton.setPosition(480, 320);
};

RoomSelectorScreen::~RoomSelectorScreen() {
};

void RoomSelectorScreen::onEnter(std::any) {
	//...
}

std::any RoomSelectorScreen::onExit() {
	//...
	return {};
}

void RoomSelectorScreen::update(float) {
	if (InputsManager::isMouseJustPressed(sf::Mouse::Left) && 
		InputsManager::getMouseScreenPos().inRect<float>(
			{ 480.f, 320.f }, 
			{ 150.f, 75.f }
		))
	{
		
		game->enterRoom(_n);
	}
}

void RoomSelectorScreen::render(sf::RenderTarget& target) {
	target.draw(_roomNumberText);
	target.draw(_infoText);
	target.draw(_triesInfoText);
	target.draw(_goSpriteButton);
	target.draw(_goTextButton);
}