#pragma once
#include "Screens/Screen.hpp"

#include <SFML/Graphics.hpp>

#include "3rd/json.hpp"

class RoomSelectorScreen : public Screen {

public: //TODO:Make this private

	sf::Text _roomNumberText;
	sf::Text _infoText;
	sf::Text _triesInfoText;

	sf::RectangleShape _bossArtworkSprite;

	sf::Text _goTextButton;				 //TODO: Make a proper UI infrastructure
	sf::RectangleShape _goSpriteButton;

	nlohmann::json _json;

	u32 _n;

public:
	RoomSelectorScreen(u32 n);
	~RoomSelectorScreen();

	virtual void onEnter();
	virtual void onExit();

	virtual void update(float dt);
	virtual void render(sf::RenderTarget& target);
};