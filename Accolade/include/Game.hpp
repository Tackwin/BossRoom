#pragma once
#include <stack>
#include <memory>

#include <SFML/Graphics.hpp>

class Screen;
class Player;
class Game {
public:
	Game();
	~Game();

	void start();

	void enterRoom(uint32_t n);
	void nextRoom();

	void enterScreen(std::unique_ptr<Screen>&& s);
	void exitScreen();

	void update(double dt);
	void updateDebugText(double dt);

	void render(sf::RenderTarget& target);

	void enterDungeon();
public: //TODO: Make this private
	uint8_t _debugTimeClockColor;
	sf::CircleShape _debugTimeClockShape;

	std::shared_ptr<Player> _player;

	std::stack<std::unique_ptr<Screen>> _screens;

	sf::Text _fpsText;
	std::map<std::string, sf::Text> _debugText;

	bool _distance = true;

};