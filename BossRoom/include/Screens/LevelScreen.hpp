#pragma once
#include <memory>

#include "Global/Const.hpp"
#include "Math/Vector2.hpp"
#include "Screens/Screen.hpp"
#include "Graphics/ParticleGenerator.hpp"


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

	ParticleGenerator _particleGenerator;

public:
	LevelScreen(uint32 n);
	~LevelScreen();

	virtual void onEnter() override;
	virtual void onExit() override;

	virtual void update(float dt) override;
	virtual void render(sf::RenderTarget& target) override;

};

