#pragma once
#include "Screen.hpp"

#include <vector>

#include "./../Common.hpp"

#include "./../Math/Vector.hpp"
#include "./../Managers/MemoryManager.hpp"

#include "./../Gameplay/Player/Player.hpp"
#include "../Ruins/Section.hpp"

class Level;
class LevelScreen : public Screen {
public:
	LevelScreen(u32 n);
	~LevelScreen();

	virtual void onEnter() override;
	virtual void onExit() override;

	virtual void update(double dt) override;
	virtual void render(sf::RenderTarget& target) override;
	void renderGui(sf::RenderTarget& target_);

	void shakeScreen(float power = 1.0f);

	u32 getIndex() const;

	std::shared_ptr<Player> getPlayer() const noexcept;
private: 
	u32 _n;

	std::shared_ptr<Player> _player;

	Vector2f _gameViewPos;
	Vector2f _gameViewSize;
	Vector2f _gameViewOffset;
	sf::View _guiView;
	sf::View _gameView;

	MM::unique_ptr<Level> _level;
	
	sf::Sprite _bossHealthTileSprite;
	std::unique_ptr<Section> _section;
	std::vector<sf::RectangleShape> _playerLife;

};

