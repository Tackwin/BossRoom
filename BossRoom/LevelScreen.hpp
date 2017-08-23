#pragma once
#include <memory>

#include "Screen.hpp"

#include "Const.hpp"
#include "Vector2.hpp"

class Level;
class Player;
class LevelScreen : public Screen {
public: //TODO: make this private
	uint32 _n;

	sf::View _guiView;
	sf::View _gameView;
	Vector2 _gameViewPos;
	Vector2 _gameViewSize;

	std::shared_ptr<Level> _level;

	sf::RectangleShape _bossLifeShape;
	std::vector<sf::RectangleShape> _playerLife;

public:
	LevelScreen(uint32 n);
	~LevelScreen();

	virtual void onEnter();
	virtual void onExit();

	virtual void update(float dt);
	virtual void render(sf::RenderTarget& target);

};

