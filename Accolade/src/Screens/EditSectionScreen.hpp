#pragma once
#include "Screen.hpp"

#include <SFML/Graphics.hpp>

#include <unordered_map>
#include <optional>
#include <memory>

#include "3rd/json.hpp"
#include "Graphics/GUI/Widget.hpp"

#include "Ruins/Section.hpp"
#include "Ruins/Structure/Plateforme.hpp"

#include "Gameplay/Characters/Slime.hpp"
#include "Gameplay/Characters/DistanceGuy.hpp"

#include "Navigation/NavigationPoint.hpp"
#include "Navigation/NavigationLink.hpp"

class Label;
class ValuePicker;
class SpriteSwitcher;
class EditSectionScreen : public Screen {
public:
	EditSectionScreen(SectionInfo section);

	virtual Type getType() const noexcept override { return edit_screen; };

	virtual void update(double dt) override;
	virtual void render(sf::RenderTarget& target) override;

	virtual void onEnter(std::any) override;
	virtual std::any onExit() override;
private:

	void deleteHovered() noexcept;

	void saveSection(std::string path) const noexcept;

	void inputSwitchState() noexcept;
	void updateCameraMovement(double dt) noexcept;

	void updatePlaceNavigationPoint() noexcept;
	void updatePlaceNavigationLink() noexcept;
	void updateDrawPlateforme() noexcept;
	void updatePlaceStartPos() noexcept;
	void updatePlaceSlime() noexcept;
	void updateNothing() noexcept;
	void updateSource() noexcept;

	void renderDebug(sf::RenderTarget& target, NavigationPointInfo info) noexcept;
	void renderDebug(sf::RenderTarget& target, NavigationLinkInfo info) noexcept;
	void renderDebug(sf::RenderTarget& target, DistanceGuyInfo info) noexcept;
	void renderDebug(sf::RenderTarget& target, PlateformeInfo info) noexcept;
	void renderDebug(sf::RenderTarget& target, SourceInfo info) noexcept;
	void renderDebug(sf::RenderTarget& target, SlimeInfo info) noexcept;

	void changeColorLabel(std::string name, Vector4f color) noexcept;

	Vector2f getSnapedMouseScreenPos() const noexcept;
	Vector2f getSnapedMouseCameraPos() const noexcept;

	SectionInfo _section;

	float _snapLevel{ 0.01f };

	std::unordered_map<std::string, Widget*> _widgets;

	SpriteSwitcher* ennemySwitcher_{ nullptr };
	SpriteSwitcher* sourceSwitcher_{ nullptr };
	ValuePicker* _savePicker{ nullptr };
	Label* _snapGrid{ nullptr };

	sf::View _uiView;
	sf::View _cameraView;

	Rectangle2f viewSize_;
	nlohmann::json _json;

	std::optional<PlateformeInfo> _newPlateforme;
	std::optional<SourceInfo> _newSource;

	std::optional<UUID> firstPoint_;

	enum EditorToolState {
		nothing,
		draw_plateforme,
		place_ennemy,
		place_start_pos,
		place_source,
		place_navigation_point,
		place_navigation_link,
		size
	} _toolState;

	void enterToolState(EditorToolState toolState) noexcept;
	void exitToolState() noexcept;

	static constexpr auto DRAW_PLATEFORME = "drawPlateforme";
	static constexpr auto PLACE_SOURCE= "placeSource";
	static constexpr auto PLACE_SLIME = "placeSlime";
	static constexpr auto PLACE_START_POS = "placeStartPos";
	static constexpr auto PLACE_NAVIGATION_POINT = "placeNavigationPoint";
	static constexpr auto PLACE_NAVIGATION_LINK = "placeNavigationLink";
};