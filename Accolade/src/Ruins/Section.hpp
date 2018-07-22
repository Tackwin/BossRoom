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
#include "./../Gameplay/Magic/Source.hpp"
#include "Gameplay/Characters/Slime.hpp"

struct SectionInfo {
	std::vector<SlimeInfo> slimes;
	std::vector<PlateformeInfo> plateformes;
	std::vector<SourceInfo> sources;
	Rectangle2f maxRectangle;
	Vector2f startPos;
	Vector2f viewSize;

	static SectionInfo loadJson(const nlohmann::json& json) noexcept;
	static nlohmann::json saveJson(SectionInfo info) noexcept;
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
	void addSource(const std::shared_ptr<Source>& ptr) noexcept;
	void addSlime(const std::shared_ptr<Slime>& ptr) noexcept;

	Vector2f getPlayerPos() const noexcept;

	std::shared_ptr<Player> getPlayer() const noexcept;

	SectionInfo getInfo() const noexcept;

private:
	// remove all object wich has the remove flag set
	void removeDeadObject() noexcept;

	void playerOnEnter(Object*) noexcept;
	void playerOnExit(Object*) noexcept;

	void subscribeToEvents() noexcept;
	void unsubscribeToEvents() noexcept;

	void pullPlayerObjects();

	nlohmann::json _levelJson;

	World _world;

	mutable sf::View _cameraView;

	std::shared_ptr<Player> _player;

	std::vector<std::shared_ptr<Slime>> _slimes;
	std::vector<std::shared_ptr<Projectile>> _projectiles;
	std::vector<std::shared_ptr<Structure>> _structures;
	std::vector<std::shared_ptr<Zone>> _playerZones;
	std::vector<std::shared_ptr<Source>> _sources;

	UUID _keyPressedEvent{ UUID::null };
	UUID _keyReleasedEvent{ UUID::null };

	SectionInfo _info;
};