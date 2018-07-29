#pragma once
#include <SFML/Graphics.hpp>
#include <optional>

#include "3rd/json.hpp"

#include "Math/Vector.hpp"
#include "Math/Rectangle.hpp"

#include "Physics/World.hpp"

#include "Structure/Structure.hpp"
#include "Structure/Plateforme.hpp"

#include "Gameplay/Magic/Sources/SourceBoomerang.hpp"
#include "Gameplay/Magic/Sources/Source.hpp"
#include "Gameplay/Magic/Spells/Spell.hpp"
#include "Gameplay/Characters/Slime.hpp"
#include "Gameplay/Player/Player.hpp"
#include "Gameplay/Boss.hpp"

struct SectionInfo {
	std::vector<SlimeInfo> slimes;
	std::vector<PlateformeInfo> plateformes;
	std::vector<SourceInfo> sources;
	std::vector<SourceBoomerangInfo> sourcesBoomerang;
	Rectangle2f maxRectangle;
	Vector2f startPos;
	Vector2f viewSize;

	static SectionInfo loadJson(const nlohmann::json& json) noexcept;
	static nlohmann::json saveJson(SectionInfo info) noexcept;
};

class Section {
public:

	Section(SectionInfo info) noexcept;

	Section(Section&) = delete;
	Section(Section&&) = delete;

	Section& operator=(Section&) = delete;
	Section& operator=(Section&&) = delete;

	void enter() noexcept;
	void exit() noexcept;

	void update(double dt) noexcept;

	void render(sf::RenderTarget& target) const noexcept;
	void renderDebug(sf::RenderTarget& target) const noexcept;

	void addStructure(const std::shared_ptr<Structure>& ptr) noexcept;
	void addSource(const std::shared_ptr<Source>& ptr) noexcept;
	void addSpell(const std::shared_ptr<Spell>& ptr) noexcept;
	void addSlime(const std::shared_ptr<Slime>& ptr) noexcept;

	Vector2f getPlayerPos() const noexcept;

	std::shared_ptr<Player> getPlayer() const noexcept;

	SectionInfo getInfo() const noexcept;

	std::shared_ptr<Object> getTargetEnnemyFromMouse() noexcept;

private:
	// remove all object wich has the remove flag set
	void removeDeadObject() noexcept;

	void playerOnEnter(Object*) noexcept;
	void playerOnExit(Object*) noexcept;

	void subscribeToEvents() noexcept;
	void unsubscribeToEvents() noexcept;

	void renderCrossOnTarget(sf::RenderTarget& target) const noexcept;

	void pullPlayerObjects();

	nlohmann::json _levelJson;

	World _world;

	mutable sf::View _cameraView;

	std::shared_ptr<Player> _player;

	std::shared_ptr<Object> targetEnnemy_;

	std::vector<std::shared_ptr<Projectile>>		_projectiles;
	std::vector<std::shared_ptr<Structure>>			_structures;
	std::vector<std::shared_ptr<Source>>			_sources;
	std::vector<std::shared_ptr<Slime>>				_slimes;
	std::vector<std::shared_ptr<Spell>>				spells_;
	std::vector<std::shared_ptr<Zone>>				_zones;

	UUID _keyPressedEvent{ UUID::null };
	UUID _keyReleasedEvent{ UUID::null };

	SectionInfo _info;
};