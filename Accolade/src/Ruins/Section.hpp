#pragma once
#include <SFML/Graphics.hpp>

#include "./../3rd/json.hpp"
#include "./../Math/Vector.hpp"
#include "./../Physics/World.hpp"
#include "./Structure/Structure.hpp"
#include "./../Gameplay/Player/Player.hpp"
#include "./../Gameplay/Boss.hpp"

class Section {
public:

	void enter() noexcept;
	void exit() noexcept;

	void loadJson(std::string jsonName) noexcept;

	void update(double dt) noexcept;

	void render(sf::RenderTarget& target) const noexcept;
	void renderDebug(sf::RenderTarget& target) const noexcept;

	void addStructure(std::shared_ptr<Structure>& ptr) noexcept;

private:
	void playerOnEnter(Object*) noexcept;

	void subscribeToEvents() noexcept;
	void unsubscribeToEvents() noexcept;

	nlohmann::json _levelJson;

	World _world;

	sf::View _cameraView;

	std::shared_ptr<Boss> _boss;
	std::shared_ptr<Player> _player;

	std::vector<std::shared_ptr<Structure>> _structures;
	std::vector<std::shared_ptr<Projectile>> _projectiles;
	std::vector<std::shared_ptr<Zone>> _playerZones;

	UUID _keyPressedEvent{ UUID::null };
	UUID _keyReleasedEvent{ UUID::null };
};