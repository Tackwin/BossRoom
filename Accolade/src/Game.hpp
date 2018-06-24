#pragma once
#include <stack>
#include <memory>

#include <SFML/Graphics.hpp>

#include "Common.hpp"

#include "Gameplay/Player/PlayerInfo.hpp"

#include "Options/KeyBindings.hpp"

class Screen;
class Player;
class Game {
public:
	Game();
	~Game();

	void start();

	void enterHeritage();

	void enterRoom(u32 n);
	void nextRoom();

	void enterScreen(std::shared_ptr<Screen> s);
	void exitScreen();

	void update(double dt);
	void updateDebugText(double dt);

	void render(sf::RenderTarget& target);

	void enterDungeon();

	void pop();
	const PlayerInfo& getPlayerInfo() const noexcept;
	void setPlayerInfo(const PlayerInfo& playerInfo) noexcept;

	const KeyBindings& getCurrentKeyBindings() const noexcept;

	PlayerInfo loadPlayerInfo(std::string character) const noexcept;
private: //TODO: Make this private
	u08 _debugTimeClockColor;
	sf::CircleShape _debugTimeClockShape;

	PlayerInfo _playerInfo;

	std::stack<std::shared_ptr<Screen>> _screens;

	sf::Text _fpsText;
	std::map<std::string, sf::Text> _debugText;

	bool _distance = true;

	std::vector<KeyBindings> _keyBindings;
	int _currentKeyBindings;
};