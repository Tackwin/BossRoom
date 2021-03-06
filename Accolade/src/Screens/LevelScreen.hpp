#pragma once
#include "Screen.hpp"

#include <vector>

#include "./../Common.hpp"

#include "./../Math/Vector.hpp"
#include "./../Managers/MemoryManager.hpp"

#include "./../Gameplay/Player/Player.hpp"
#include "../Ruins/Section.hpp"

#include "Ruins/Instance.hpp"
#include "GUI/Inventory/Inventory.hpp"

class Level;
class Label;
class LevelScreen : public Screen {
public:
	LevelScreen(u32 n);
	~LevelScreen();

	virtual Type getType() const noexcept override { return level_screen; }

	virtual void onEnter(std::any) override;
	virtual std::any onExit() override;

	virtual void update(double dt) override;
	virtual void render(sf::RenderTarget& target) override;
	void renderGui(sf::RenderTarget& target_);
	void render_instance_map(sf::RenderTarget& target) noexcept;

	void shakeScreen(float power = 1.0f);

	u32 getIndex() const;
private: 
	u32 _n;

	Vector2f _gameViewPos;
	Vector2f _gameViewSize;
	Vector2f _gameViewOffset;
	sf::View _guiView;
	sf::View _gameView;

	std::unique_ptr<Instance> instance;

	std::unique_ptr<Label> section_id_label{ nullptr };
	sf::Sprite _bossHealthTileSprite;
	std::vector<sf::RectangleShape> _playerLife;

	std::unique_ptr<Inventory> inventory;

	bool inventory_is_up{ false };
	bool instance_map_is_up{ false };

	sf::RenderTexture instance_map_render_texture;
	sf::View instance_map_view;
};

