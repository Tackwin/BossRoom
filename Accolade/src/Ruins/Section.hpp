#pragma once
#include <SFML/Graphics.hpp>

#include "./../3rd/json.hpp"
#include "./../Math/Vector.hpp"
#include "./../Physics/World.hpp"
#include "./Structure/Structure.hpp"
#include "./../Gameplay/Player/Player.hpp"
#include "./../Gameplay/Boss.hpp"
#include "../Math/Rectangle.hpp"
#include "Structure/Plateforme.hpp"


struct SectionInfo {
	std::vector<PlateformeInfo> plateformes;
	Rectangle2f maxRectangle;
	Vector2f startPos;

	static SectionInfo load(const nlohmann::json& json) noexcept;
};

class Section {
public:

	Section(SectionInfo info) noexcept;

	void enter() noexcept;
	void exit() noexcept;

	void update(double dt) noexcept;

	void render(sf::RenderTarget& target) const noexcept;
	void renderDebug(sf::RenderTarget& target) const noexcept;

	void addStructure(const std::shared_ptr<Structure>& ptr) noexcept;

private:
	void playerOnEnter(Object*) noexcept;
	void playerOnExit(Object*) noexcept;

	void subscribeToEvents() noexcept;
	void unsubscribeToEvents() noexcept;

	nlohmann::json _levelJson;

	World _world;

	sf::View _cameraView;

	std::shared_ptr<Player> _player;

	std::vector<std::shared_ptr<Structure>> _structures;
	std::vector<std::shared_ptr<Projectile>> _projectiles;
	std::vector<std::shared_ptr<Zone>> _playerZones;

	UUID _keyPressedEvent{ UUID::null };
	UUID _keyReleasedEvent{ UUID::null };

	SectionInfo _info;
};