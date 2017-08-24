#pragma once
#include <memory>

#include "Global/Const.hpp"
#include "Math/Vector2.hpp"
#include "Screens/Screen.hpp"

class Level;
class Player;
class Particle;
class LevelScreen : public Screen {
public: //TODO: make this private
	uint32 _n;

	sf::View _guiView;
	sf::View _gameView;
	Vector2 _gameViewPos;
	Vector2 _gameViewSize;

	std::shared_ptr<Level> _level;
	
	std::vector<std::shared_ptr<Particle>> _particles;

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

