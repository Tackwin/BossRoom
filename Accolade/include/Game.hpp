#pragma once
#include <stack>
#include <memory>

#include <SFML/Graphics.hpp>

#include "Common.hpp"

class Screen;
class Player;
class Game {
public:
	Game();
	~Game();

	void start();

	void enterRoom(u32 n);
	void nextRoom();

	void enterScreen(std::shared_ptr<Screen> s);
	void exitScreen();

	void update(double dt);
	void updateDebugText(double dt);

	Player* getPlayer() const;

	void render(sf::RenderTarget& target);

	void enterDungeon();

	void pop();
public: //TODO: Make this private
	u08 _debugTimeClockColor;
	sf::CircleShape _debugTimeClockShape;

	std::shared_ptr<Player> _player;

	std::stack<std::shared_ptr<Screen>> _screens;

	sf::Text _fpsText;
	std::map<std::string, sf::Text> _debugText;

	bool _distance = true;

};