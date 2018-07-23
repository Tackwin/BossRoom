#pragma once
#include "Screen.hpp"

#include <SFML/Graphics.hpp>

#include "./../Common.hpp"

#include "./../3rd/json.hpp"

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
	virtual Type getType() const noexcept override { return edit_screen; }

	virtual void onEnter(std::any) override;
	virtual std::any onExit() override;

	virtual void update(float dt);
	virtual void render(sf::RenderTarget& target);
};