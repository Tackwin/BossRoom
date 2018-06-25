#pragma once
#include "Screen.hpp"

#include <memory>
#include <vector>
#include <unordered_map>

#include <SFML/Graphics.hpp>

#include "../3rd/json.hpp"

#include "../Graphics/GUI/Widget.hpp"
#include "../Graphics/GUI/Label.hpp"
#include "../Graphics/GUI/Panel.hpp"
#include "../Graphics/GUI/Button.hpp"
#include "../Graphics/GUI/GridLayout.hpp"

#include "./../Managers/MemoryManager.hpp"

#include "../Physics/World.hpp"

#include "../GUI/Shop.hpp"

class Zone;
class Player;
class Projectile;
class StartScreen : public Screen {
public:
	StartScreen();

	virtual void onEnter();
	virtual void onExit();

	virtual void update(double dt);
	virtual void render(sf::RenderTarget& target);
	void renderGui(sf::RenderTarget& target);

	void enterShop();
	void leaveShop();
private:

	void subscribeToEvents() noexcept;
	void unsubscribeToEvents() noexcept;

	void pullPlayerObjects();

	void initializeWorld();
	void initializeGui();
	void initializeSprite();

	void removeNeeded();

	void activateShop();
	void unActivateShop();

	void playerOnEnter(Object* object);
	void playerOnExit(Object* object);

	void projectileOnEnter(std::weak_ptr<Projectile> projectile, Object* object);
	void projectileOnExit(std::weak_ptr<Projectile> projectile, Object* object);

	sf::Sprite _startBackground;
	sf::Sprite _dungeon;
	sf::Sprite _dungeonDoor;
	sf::Sprite _merchantShop;

	sf::View _guiView;
	sf::View _playerView;

	nlohmann::json _json;

	std::vector<std::shared_ptr<Projectile>> _projectiles;
	std::vector<std::shared_ptr<Zone>> _playerZones;
	std::shared_ptr<Player> _player;

	bool _isInShop = false;
	bool _enteredShop = false;

	World _world;

	std::map<std::string, std::shared_ptr<Zone>> _zones;

	std::shared_ptr<Object> _floor;

	Widget _guiRoot;
	Panel _weaponIcon;
	Label _weaponLabel;

	Shop _shop;

	u32 _keyPressedEvent{ 0u };
	u32 _keyReleasedEvent{ 0u };
};
