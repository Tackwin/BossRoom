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

class EditSectionScreen : public Screen {
public:
	EditSectionScreen(SectionInfo section);

	void update(double dt);
	void render(sf::RenderTarget& target);

	void onEnter();
	void onExit();
private:

	void inputSwitchState() noexcept;
	void updateCameraMovement(double dt) noexcept;

	void updateDrawPlateforme() noexcept;
	void updatePlaceSlime() noexcept;

	void renderDebug(sf::RenderTarget& target, PlateformeInfo info) noexcept;
	void renderDebug(sf::RenderTarget& target, SlimeInfo info) noexcept;

	void changeColorLabel(std::string name, Vector4f color) noexcept;

	SectionInfo _section;

	std::unordered_map<std::string, Widget*> _widgets;

	std::vector<PlateformeInfo> _plateformes;
	std::vector<SlimeInfo> _slimes;

	sf::View _uiView;
	sf::View _cameraView;

	nlohmann::json _json;

	std::optional<PlateformeInfo> _newPlateforme;
	std::optional<SlimeInfo> _newSlime;

	enum EditorToolState {
		nothing,
		draw_plateforme,
		place_slime,
		size
	} _toolState;

	void enterToolState(EditorToolState toolState) noexcept;
	void exitToolState() noexcept;

	static constexpr auto DRAW_PLATEFORME = "drawPlateforme";
	static constexpr auto PLACE_SLIME = "placeSlime";
};