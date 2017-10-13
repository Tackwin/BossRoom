#pragma once
#include <memory>

#include <SFML/Graphics.hpp>

#include "Math/Vec.hpp"

#include "Screens/Screen.hpp"

class Level;
class Panel;
class Player;
class LevelScreen : public Screen {
public:
	LevelScreen(uint32_t n);
	~LevelScreen();

	virtual void onEnter() override;
	virtual void onExit() override;

	virtual void update(double dt) override;
	virtual void render(sf::RenderTarget& target) override;
	void renderGui(sf::RenderTarget& target_);

	void shakeScreen(float power = 1.0f);

	uint32_t getIndex() const;
private: 
	uint32_t _n;

	sf::View _guiView;
	sf::View _gameView;
	Vector2f _gameViewPos;
	Vector2f _gameViewSize;
	Vector2f _gameViewOffset;

	std::unique_ptr<Level> _level;
	
	sf::Sprite _bossHealthTileSprite;
	std::vector<sf::RectangleShape> _playerLife;

	std::vector<std::unique_ptr<Panel>> _panels;
};

