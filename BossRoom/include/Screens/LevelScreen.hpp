#pragma once
#include <memory>

#include <SFML/Graphics.hpp>

#include "Global/Const.hpp"
#include "Math/Vector2.hpp"
#include "Screens/Screen.hpp"

class Level;
class Player;
class LevelScreen : public Screen {
public: //TODO: make this private
	uint32 _n;

	sf::View _guiView;
	sf::View _gameView;
	Vector2 _gameViewPos;
	Vector2 _gameViewSize;
	Vector2 _gameViewOffset;

	std::shared_ptr<Level> _level;
	
	sf::Sprite _bossHealthTileSprite;
	std::vector<sf::RectangleShape> _playerLife;

public:
	LevelScreen(uint32 n);
	~LevelScreen();

	virtual void onEnter() override;
	virtual void onExit() override;

	virtual void update(float dt) override;
	virtual void render(sf::RenderTarget& target) override;
	void renderGui(sf::RenderTarget& target_);

	void shakeScreen(float power = 1.0f);
};

