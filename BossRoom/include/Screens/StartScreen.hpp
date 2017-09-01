#pragma once
#include "Screen.hpp"

#include <memory>

#include <SFML/Graphics.hpp>

#include "3rd/json.hpp"

class Player;
class StartScreen : public Screen {
public:
	StartScreen();

	virtual void onEnter();
	virtual void onExit();

	virtual void update(float dt);
	virtual void render(sf::RenderTarget& target);
private:

	void initializeSprite();

	sf::Sprite _startBackground;
	sf::Sprite _dungeon;
	sf::Sprite _dungeonDoor;
	sf::Sprite _merchantShop;

	sf::View _guiView;
	sf::View _playerView;

	nlohmann::json _json;

	std::shared_ptr<Player> _player;
};
