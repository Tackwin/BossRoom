#pragma once
#include <memory>
#include <stack>

#include <SFML/Graphics.hpp>

#include <enTT/registry.hpp>

#include "Global/Const.hpp"

class Screen;
class Player;
class Game {
public:
	Game();
	~Game();

	void start();

	void enterRoom(uint32 n);
	void nextRoom();

	void enterScreen(std::shared_ptr<Screen> s);
	void exitScreen();

	void update(float dt);
	void updateDebugText(float dt);

	void render(sf::RenderTarget& target);

	void enterDungeon();
public: //TODO: Make this private
	uint8_t _debugTimeClockColor;
	sf::CircleShape _debugTimeClockShape;

	std::shared_ptr<Player> _player;

	std::stack<std::shared_ptr<Screen>> _screens;

	sf::Text _fpsText;
	std::map<std::string, sf::Text> _debugText;

	bool _distance = true;

};