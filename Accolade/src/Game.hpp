#pragma once
#include <any>
#include <stack>
#include <memory>

#include <SFML/Graphics.hpp>

#include "Common.hpp"

#include "Options/KeyBindings.hpp"

#include "Gameplay/Player/Player.hpp"
#include "Screens/Screen.hpp"
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

	void update(double dt) noexcept;
	void updateDebugText(double dt);

	void render(sf::RenderTarget& target);

	void enterDungeon();

	void pop();
	const PlayerInfo& getPlayerInfo() const noexcept;
	void setPlayerInfo(const PlayerInfo& playerInfo) noexcept;

	const KeyBindings& getCurrentKeyBindings() const noexcept;

	Screen::Type getLastScreen() const noexcept;

	PlayerInfo loadPlayerInfo(std::string character) const noexcept;
private:

	std::any _returnedFromScreen;

	u08 _debugTimeClockColor;
    sf::CircleShape _debugTimeClockShape;

	PlayerInfo _playerInfo;

	std::stack<std::shared_ptr<Screen>> _screens;

	sf::Text _fpsText;
	std::map<std::string, sf::Text> _debugText;

	bool _distance = true;

	std::vector<KeyBindings> _keyBindings;
	int _currentKeyBindings;

	Screen::Type _lastScreen;
};
