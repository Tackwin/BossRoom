#pragma once
#include "Screen.hpp"

#include <memory>
#include <vector>

#include <SFML/Graphics.hpp>

#include "3rd/json.hpp"

#include <Graphics/GUI/Widget.hpp>
#include <Graphics/GUI/Label.hpp>
#include <Graphics/GUI/Panel.hpp>
#include <Graphics/GUI/Button.hpp>
#include <Graphics/GUI/GridLayout.hpp>


class Player;
class Projectile;
class StartScreen : public Screen {
public:
	StartScreen();

	virtual void onEnter();
	virtual void onExit();

	virtual void update(float dt);
	virtual void render(sf::RenderTarget& target);
	void renderGui(sf::RenderTarget& target);

	void enterShop();
	void leaveShop();
private:

	void initializeSprite();

	sf::Sprite _startBackground;
	sf::Sprite _dungeon;
	sf::Sprite _dungeonDoor;
	sf::Sprite _merchantShop;

	sf::View _guiView;
	sf::View _playerView;

	nlohmann::json _json;

	std::vector<std::shared_ptr<Projectile>> _projectiles;
	std::shared_ptr<Player> _player;

	bool _isInShop = false;

	Button button;

};
