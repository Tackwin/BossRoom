#include "EditSectionScreen.hpp"

#include <fstream>

#include "Managers/InputsManager.hpp"
#include "Managers/AssetsManager.hpp"

#include "Game.hpp"

#include "LevelScreen.hpp"

#include "Graphics/GUI/Label.hpp"
#include "Graphics/GUI/ValuePicker.hpp"

EditSectionScreen::EditSectionScreen(SectionInfo section) :
	Screen(), _section(section),
	_uiView({ WIDTH / 2.f, HEIGHT / 2.f }, { (float)WIDTH, (float)HEIGHT })
{
	_cameraView.setCenter(_section.maxRectangle.fitUpRatio(RATIO).center());
	_cameraView.setSize(_section.viewSize.fitUpRatio(RATIO));
}

void EditSectionScreen::update(double dt) {
	if (IM::isKeyPressed(sf::Keyboard::LControl) &&
		IM::isKeyJustPressed(sf::Keyboard::E)
	) {
		saveSection("rooms/roomA.json");
		C::game->exitScreen();
		return;
	}
	inputSwitchState();
	_widgets.at("root")->propagateInput();

	switch (_toolState)
	{
	case draw_plateforme:
		updateDrawPlateforme();
		break;
	case place_slime:
		updatePlaceSlime();
		break;
	case nothing:
		updateNothing();
		break;
	default:
		break;
	}

	updateCameraMovement(dt);
}

void EditSectionScreen::updatePlaceSlime() noexcept {
	_newSlime->startPos = IM::getMousePosInView(_cameraView);
	if (IM::isMouseJustPressed(sf::Mouse::Left)) {
		_section.slimes.push_back(*_newSlime);
	}
}
void EditSectionScreen::updateDrawPlateforme() noexcept {
	if (IM::isMouseJustPressed(sf::Mouse::Left)) {
		switch (_toolState) {
		case draw_plateforme:
			auto p = PlateformeInfo();
			p.rectangle.pos = IM::getMousePosInView(_cameraView);

			_newPlateforme = p;
			break;
		}
	}
	else if (IM::isMouseJustReleased(sf::Mouse::Left)) {
		switch (_toolState)
		{
		case draw_plateforme: {
			if (_newPlateforme->rectangle.w < 0.f) {
				_newPlateforme->rectangle.x += _newPlateforme->rectangle.w;
				_newPlateforme->rectangle.w *= -1;
			} if (_newPlateforme->rectangle.h < 0.f) {
				_newPlateforme->rectangle.y += _newPlateforme->rectangle.h;
				_newPlateforme->rectangle.h *= -1;
			}

			_section.plateformes.push_back(_newPlateforme.value());
			std::vector<Rectangle2f> rectangles;
			std::transform(
				std::begin(_section.plateformes),
				std::end(_section.plateformes),
				std::back_inserter(rectangles),
				[](PlateformeInfo info) {
					return info.rectangle;
				}
			);
			_section.maxRectangle = Rectangle2f::hull(rectangles);
			_newPlateforme.reset();
		} break;
		default:
			break;
		}
	}

	if (_newPlateforme.has_value()) {
		_newPlateforme->rectangle.size =
			IM::getMousePosInView(_cameraView) - _newPlateforme->rectangle.pos;
	}
}

void EditSectionScreen::updateNothing() noexcept {
	
}

void EditSectionScreen::updateCameraMovement(double dt) noexcept {
	if (IM::isKeyPressed(sf::Keyboard::Left)) {
		_cameraView.move({ -(float)(dt * _section.maxRectangle.w / 5.f), 0.f });
	}
	if (IM::isKeyPressed(sf::Keyboard::Right)) {
		_cameraView.move({ +(float)(dt * _section.maxRectangle.w / 5.f), 0.f });
	}
	if (IM::isKeyPressed(sf::Keyboard::Up)) {
		_cameraView.move({ 0.f, -(float)(dt * _section.maxRectangle.h / 5.f) });
	}
	if (IM::isKeyPressed(sf::Keyboard::Down)) {
		_cameraView.move({ 0.f, +(float)(dt * _section.maxRectangle.h / 5) });
	}
}
void EditSectionScreen::inputSwitchState() noexcept {
	if (IM::isLastSequenceJustFinished({
		sf::Keyboard::LControl, sf::Keyboard::Q, sf::Keyboard::P
	})) {
		if (_toolState != draw_plateforme) {
			exitToolState();
			enterToolState(draw_plateforme);
		}
		else {
			exitToolState();
		}
	}


	else if (IM::isLastSequenceJustFinished({
		sf::Keyboard::LControl, sf::Keyboard::Q, sf::Keyboard::S
	})){
		if (_toolState != place_slime) {
			exitToolState();
			enterToolState(place_slime);
		}
		else {
			exitToolState();
		}
	}
	else if (IM::isKeyJustPressed(sf::Keyboard::Escape) && _toolState != nothing) {
		exitToolState();
	}
}

void EditSectionScreen::render(sf::RenderTarget& target) {
	auto old = target.getView();
	
	target.setView(_cameraView);
	for (auto plateforme : _section.plateformes) {
		renderDebug(target, plateforme);
	}
	for (auto slime : _section.slimes) {
		renderDebug(target, slime);
	}

	if (_newPlateforme.has_value()) {
		renderDebug(target, *_newPlateforme);
	}
	if (_newSlime.has_value()) {
		renderDebug(target, *_newSlime);
	}

	target.setView(_uiView);
	_widgets.at("root")->propagateRender(target);

	target.setView(old);
}

void EditSectionScreen::onEnter() {
	_json = AM::getJson("editScreen");

	for (auto&[key, value] : _json.get<nlohmann::json::object_t>()) {
		auto json = value;
		if (auto it = json.find("type"); it != json.end()) {
			auto type = it->get<std::string>();
			if (type == Label::NAME) {
				_widgets[key] = new Label(json);
			}
			if (type == ValuePicker::NAME) {
				_widgets[key] = new ValuePicker(json);
			}
		}
		else {
			_widgets[key] = new Widget(json);
		}
	}
}

void EditSectionScreen::onExit() {
	for (auto&[key, value] : _widgets) {
		delete value;
	}
}

void EditSectionScreen::enterToolState(
	EditSectionScreen::EditorToolState toolState
) noexcept {
	_toolState = toolState;
	switch (toolState)
	{
	case draw_plateforme:
		changeColorLabel(DRAW_PLATEFORME, { 0.0, 1.0, 0.0, 1.0 });
		break;
	case place_slime:
		_newSlime = SlimeInfo();
		_newSlime->startPos = IM::getMousePosInView(_cameraView);
		changeColorLabel(PLACE_SLIME, { 0.0, 1.0, 0.0, 1.0 });
		break;
	default:
		break;
	}
}

void EditSectionScreen::exitToolState() noexcept {
	switch (_toolState)
	{
	case draw_plateforme:
		_newPlateforme.reset();
		changeColorLabel(DRAW_PLATEFORME, { 1.0, 1.0, 1.0, 1.0 });
		break;
	case place_slime:
		_newSlime.reset();
		changeColorLabel(PLACE_SLIME, { 1.0, 1.0, 1.0, 1.0 });
		break;
	default:
		break;
	}
	_toolState = nothing;
}

void EditSectionScreen::renderDebug(
	sf::RenderTarget& target, PlateformeInfo info
) noexcept {
	info.rectangle.render(target, { 1.0, 1.0, 0.0, 1.0 });
}
void EditSectionScreen::renderDebug(
	sf::RenderTarget& target, SlimeInfo info
) noexcept {
	info.startPos.plot(
		target, info.size.x / 2.f, { 1.0,1.0, 1.0, 1.0 }, {0.0, 0.0, 0.0, 0.0}, 0.f
	);
}

void EditSectionScreen::changeColorLabel(std::string name, Vector4f color) noexcept {
	auto root = _widgets.at("root");

	if (auto it = root->findChild(name); dynamic_cast<const Label*>(it)) {
		((Label*)it)->setTextColor(color);
	}
}

void EditSectionScreen::saveSection(std::string path) const noexcept {
	auto json = SectionInfo::saveJson(_section);
	std::ofstream file;
	file.open(ASSETS_PATH + path);

	file << json;

	file.close();
}