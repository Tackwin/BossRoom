#pragma once
#include <SFML/Graphics.hpp>
#include <optional>
#include <unordered_set>

#include "3rd/json.hpp"

#include "Math/Vector.hpp"
#include "Math/Rectangle.hpp"

#include "Physics/World.hpp"

#include "Structure/Portal.hpp"
#include "Structure/Structure.hpp"
#include "Structure/Plateforme.hpp"
#include "Structure/SemiPlateforme.hpp"

#include "Navigation/NavigationPoint.hpp"
#include "Navigation/NavigationLink.hpp"

#include "Gameplay/Magic/Sources/SourceDirection.hpp"
#include "Gameplay/Magic/Sources/SourceTarget.hpp"
#include "Gameplay/Magic/Sources/SourceVaccum.hpp"
#include "Gameplay/Magic/Sources/Source.hpp"
#include "Gameplay/Magic/Spells/Spell.hpp"
#include "Gameplay/Characters/DistanceGuy.hpp"
#include "Gameplay/Characters/FirstBoss.hpp"
#include "Gameplay/Characters/MeleeGuy.hpp"
#include "Gameplay/Characters/Slime.hpp"
#include "Gameplay/Characters/Fly.hpp"
#include "Gameplay/Player/Player.hpp"
#include "Gameplay/Item/Loot.hpp"
#include "Gameplay/Boss.hpp"

#include "GUI/Inventory/Inventory.hpp"

#include "RTTI/dyn_struct.hpp"

struct SectionInfo {
	std::vector<NavigationPointInfo> navigationPoints;
	std::vector<NavigationLinkInfo> navigationLinks;

	std::vector<DistanceGuyInfo> distanceGuys;
	std::vector<FirstBossInfo> first_bosses;
	std::vector<MeleeGuyInfo> meleeGuys;
	std::vector<SlimeInfo> slimes;
	std::vector<FlyInfo> flies;

	std::vector<SemiPlateformeInfo> semiPlateformes;
	std::vector<PlateformeInfo> plateformes;
	std::vector<PortalInfo> portals;

	std::vector<SourceInfo> sources;
	std::vector<SourceTargetInfo> sourcesBoomerang;
	std::vector<SourceVaccumInfo> sourcesVaccum;
	std::vector<SourceDirectionInfo> sourcesDirection;

	dyn_struct meta_data;

	Rectangle2f maxRectangle;
	Vector2f startPos;
	Vector2f viewSize;

	static SectionInfo loadJson(const nlohmann::json& json) noexcept;
	static nlohmann::json saveJson(SectionInfo info) noexcept;
};

extern void to_dyn_struct(dyn_struct& d_struct, const SectionInfo& info) noexcept;
extern void from_dyn_struct(const dyn_struct& d_struct, SectionInfo& info) noexcept;

class Section {
public:

	Section(SectionInfo info) noexcept;

	Section(Section&) = delete;
	Section(Section&&) = default;

	Section& operator=(Section&) = delete;
	Section& operator=(Section&&) = default;

	void enter() noexcept;
	void exit() noexcept;

	void update(double dt) noexcept;

	void render(sf::RenderTarget& target) const noexcept;
	void renderDebug(sf::RenderTarget& target) const noexcept;

	void addZone(std::shared_ptr<Zone> ptr) noexcept;

	void addStructure(const std::shared_ptr<Structure>& ptr) noexcept;

	void addSource(const std::shared_ptr<Source>& ptr) noexcept;

	void addSpell(const std::shared_ptr<Spell>& ptr) noexcept;

	void addDistanceGuy(const std::shared_ptr<DistanceGuy>& ptr) noexcept;
	void addMeleeGuy(const std::shared_ptr<MeleeGuy>& ptr) noexcept;
	void addSlime(const std::shared_ptr<Slime>& ptr) noexcept;
	void addFly(const std::shared_ptr<Fly>& ptr) noexcept;

	void addEnnemy(const FirstBossInfo& info) noexcept;

	// Get NavigationPoint from id
	NavigationPointInfo getNavigationPoint(UUID id) const noexcept;
	// Get NavigationPoint from a link, specifying the one we _don't_ want
	NavigationPointInfo getNavigationPoint(UUID link, UUID current) const noexcept;
	NavigationPointInfo getNextNavigationPointFrom(UUID id, Vector2f to) const noexcept;

	const std::vector<PortalInfo>& getAllPortals() const noexcept;
	const PortalInfo& getPortalInSpot(size_t spot) const noexcept;
	const PortalInfo& getPortal(UUID id) const noexcept;

	Vector2f getPlayerPos() const noexcept;
	void setPlayerPos(Vector2f) const noexcept;

	std::shared_ptr<Player> getPlayer() const noexcept;

	SectionInfo getInfo() const noexcept;

	std::shared_ptr<Object> getTargetEnnemyFromMouse() noexcept;
	std::weak_ptr<Object> getObject(UUID id) const noexcept;

	sf::View getCameraView() const noexcept;

	void setFileName(std::filesystem::path fileName) noexcept;
	std::filesystem::path getFileName() const noexcept;

	void setFilepath(std::filesystem::path filepath) noexcept;
	std::filesystem::path getFilepath() const noexcept;

	double getTimeSinceEntered() const noexcept;

	void spawnLoot(std::shared_ptr<Loot> loot) noexcept;

	void startAimAnimation() noexcept;
	void stopAimAnimation() noexcept;

	std::optional<RayCollision> ray_cast(
		const Rayf& ray, std::bitset<Object::SIZE> mask = xstd::full_bitset<Object::SIZE>()
	) const noexcept;
private:

	void updateAimAnimation(double dt) noexcept;

		// remove all object wich has the remove flag set
	void removeDeadObject() noexcept;

	void playerOnEnter(Object*) noexcept;
	void playerOnExit(Object*) noexcept;

	void subscribeToEvents() noexcept;
	void unsubscribeToEvents() noexcept;

	void renderCrossOnTarget(sf::RenderTarget& target) const noexcept;

	void pullZonesFromObjects() noexcept;
	void pullProjectilsFromObjects() noexcept;

	NavigationPointInfo getClosestNavigationPoint(Vector2f p) const noexcept;

	double time_since_entered{ 0.f };

	nlohmann::json _levelJson;

	std::filesystem::path fileName_;
	std::filesystem::path filepath_;

	World _world;

	mutable sf::View _cameraView;

	std::shared_ptr<Player> _player;

	std::shared_ptr<Object> targetEnnemy_;

	std::vector<std::shared_ptr<Source>>			_sources;

	std::vector<std::shared_ptr<Projectile>>		_projectiles;

	std::vector<std::shared_ptr<Structure>>			_structures;

	std::vector<std::shared_ptr<Spell>>				spells_;

	std::vector<std::shared_ptr<Zone>>				_zones;
	std::vector<std::shared_ptr<Loot>>				loots;

	std::vector<std::shared_ptr<DistanceGuy>>		distanceGuys_;
	std::vector<std::shared_ptr<FirstBoss>>			first_bosses;
	std::vector<std::shared_ptr<MeleeGuy>>			meleeGuys_;
	std::vector<std::shared_ptr<Slime>>				_slimes;
	std::vector<std::shared_ptr<Fly>>				flies;

	bool aiming{ false };
	float elastic_force_aim_sprite{ 1.f };
	sf::Sprite aim_sprite;

	UUID _keyPressedEvent{ UUID::zero() };
	UUID _keyReleasedEvent{ UUID::zero() };

	SectionInfo _info;

	std::unique_ptr<Inventory> inventory;
};



extern std::unordered_set<size_t> get_all_accessible_dir(const SectionInfo& info) noexcept;
