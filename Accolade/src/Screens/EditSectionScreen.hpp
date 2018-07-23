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

class ValuePicker;
class Label;
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

	void updateDrawPlateforme() noexcept;
	void updatePlaceStartPos() noexcept;
	void updatePlaceSlime() noexcept;
	void updateNothing() noexcept;
	void updateSource() noexcept;

	void renderDebug(sf::RenderTarget& target, PlateformeInfo info) noexcept;
	void renderDebug(sf::RenderTarget& target, SlimeInfo info) noexcept;

	void changeColorLabel(std::string name, Vector4f color) noexcept;

	Vector2f getSnapedMouseScreenPos() const noexcept;
	Vector2f getSnapedMouseCameraPos() const noexcept;

	SectionInfo _section;

	float _snapLevel{ 0.01f };

	std::unordered_map<std::string, Widget*> _widgets;

	ValuePicker* _savePicker{ nullptr };
	Label* _snapGrid{ nullptr };

	sf::View _uiView;
	sf::View _cameraView;

	nlohmann::json _json;

	std::optional<PlateformeInfo> _newPlateforme;
	std::optional<SlimeInfo> _newSlime;

	enum EditorToolState {
		nothing,
		draw_plateforme,
		place_slime,
		place_start_pos,
		place_source,
		size
	} _toolState;

	void enterToolState(EditorToolState toolState) noexcept;
	void exitToolState() noexcept;

	static constexpr auto DRAW_PLATEFORME = "drawPlateforme";
	static constexpr auto PLACE_SLIME = "placeSlime";
	static constexpr auto PLACE_START_POS = "placeStartPos";
};