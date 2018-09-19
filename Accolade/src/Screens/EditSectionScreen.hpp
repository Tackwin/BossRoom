#pragma once
#include "Screen.hpp"

#include <SFML/Graphics.hpp>

#include <unordered_map>
#include <filesystem>
#include <optional>
#include <memory>
#include <mutex>
#include <atomic>

#include "3rd/json.hpp"
#include "Graphics/GUI/Widget.hpp"

#include "Ruins/Section.hpp"
#include "Ruins/Structure/Plateforme.hpp"

#include "Gameplay/Characters/Slime.hpp"
#include "Gameplay/Characters/DistanceGuy.hpp"

#include "Navigation/NavigationPoint.hpp"
#include "Navigation/NavigationLink.hpp"

class Label;
class Panel;
class ValuePicker;
class SpriteSwitcher;
class EditSectionScreen : public Screen {
public:
	using ReturnType = std::tuple<SectionInfo, std::filesystem::path>;

	EditSectionScreen(Section* section);

	virtual Type getType() const noexcept override { return edit_screen; };

	virtual void update(double dt) override;
	virtual void render(sf::RenderTarget& target) override;

	virtual void onEnter(std::any) override;
	virtual std::any onExit() override;
private:

	void selectFocus() noexcept;

	template<typename F>
	void applyToFocused(F&& f) noexcept {
		if (!currentlyFocused) return;

		auto id = currentlyFocused->first;
		auto ptr = currentlyFocused->second;
#define X(x) if (id == x::JSON_ID) f((x*)ptr)

		X(NavigationPointInfo);
		X(NavigationLinkInfo);
		X(SourceDirectionInfo);
		X(SourceTargetInfo);
		X(SourceVaccumInfo);
		X(DistanceGuyInfo);
		X(PlateformeInfo);
		X(MeleeGuyInfo);
		X(PortalInfo);
		X(SlimeInfo);
		X(FlyInfo);

#undef X
	}

	void deleteHovered() noexcept;

	void saveSection(std::filesystem::path path) const noexcept;

	void inputSwitchState() noexcept;
	void updateCameraMovement(double dt) noexcept;

	void updatePlaceNavigationPoint() noexcept;
	void updatePlaceNavigationLink() noexcept;
	void updatePlaceStructure() noexcept;
	void updateDrawPlateform() noexcept;
	void updatePlaceStartPos() noexcept;
	void updateDrawPortal() noexcept;
	void updatePlaceSlime() noexcept;
	void updateNothing() noexcept;
	void updateSource() noexcept;

	void renderDebug(sf::RenderTarget& target, NavigationPointInfo info) noexcept;
	void renderDebug(sf::RenderTarget& target, NavigationLinkInfo info) noexcept;
	void renderDebug(sf::RenderTarget& target, DistanceGuyInfo info) noexcept;
	void renderDebug(sf::RenderTarget& target, PlateformeInfo info) noexcept;
	void renderDebug(sf::RenderTarget& target, MeleeGuyInfo info) noexcept;
	void renderDebug(sf::RenderTarget& target, SourceInfo info) noexcept;
	void renderDebug(sf::RenderTarget& target, PortalInfo info) noexcept;
	void renderDebug(sf::RenderTarget& target, SlimeInfo info) noexcept;
	void renderDebug(sf::RenderTarget& target, FlyInfo info) noexcept;

	void loadSectionFile(std::filesystem::path path) noexcept;

	void changeColorLabel(std::string name, Vector4f color) noexcept;

	void constructUI() noexcept;

	Vector2f getSnapedMouseScreenPos() const noexcept;
	Vector2f getSnapedMouseCameraPos() const noexcept;

	bool onClickEndedConfirmJsonEditPanel() noexcept;

private:
	SectionInfo sectionInfo_;

	float _snapLevel{ 0.01f };

	std::unordered_map<std::string, Widget*> _widgets;

	SpriteSwitcher* ennemySwitcher_{ nullptr };
	SpriteSwitcher* sourceSwitcher_{ nullptr };
	SpriteSwitcher* structureSwitcher{ nullptr };
	Panel* jsonEditPanel{ nullptr };
	ValuePicker* _savePicker{ nullptr };
	Label* _snapGrid{ nullptr };

	sf::View _uiView;
	sf::View _cameraView;

	Rectangle2f viewSize_;
	nlohmann::json _json;


	// Ok so what's the deal with this one ?
	// first element is the JSON_ID of the things pointed by second
	// it's meant to be used to cast it.
	std::optional<std::pair<std::string, void*>> currentlyFocused{ std::nullopt };

	std::optional<PlateformeInfo> _newPlateforme;
	std::optional<SourceInfo> _newSource;

	std::optional<UUID> firstPoint_;

	std::optional<Vector2f> portalFirstPoint{ std::nullopt };

	std::filesystem::path filepath_;

	bool draggingScreen{ false };

	std::atomic<bool> toLoadFile{ false };
	std::mutex fileToLoadMutex;
	std::filesystem::path fileToLoad;

	enum EditorToolState {
		nothing,
		place_structure,
		place_ennemy,
		place_start_pos,
		place_source,
		place_navigation_point,
		place_navigation_link,
		size
	} _toolState{ nothing };

	void enterToolState(EditorToolState toolState) noexcept;
	void exitToolState() noexcept;

	static constexpr auto PLACE_STRUCTURE = "placeStructure";
	static constexpr auto PLACE_SOURCE= "placeSource";
	static constexpr auto PLACE_SLIME = "placeSlime";
	static constexpr auto PLACE_START_POS = "placeStartPos";
	static constexpr auto PLACE_NAVIGATION_POINT = "placeNavigationPoint";
	static constexpr auto PLACE_NAVIGATION_LINK = "placeNavigationLink";
};