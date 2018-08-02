#include "EditSectionScreen.hpp"

#include <fstream>

#include "Managers/InputsManager.hpp"
#include "Managers/AssetsManager.hpp"

#include "Game.hpp"

#include "LevelScreen.hpp"

#include "Graphics/GUI/Switcher.hpp"
#include "Graphics/GUI/Label.hpp"
#include "Graphics/GUI/ValuePicker.hpp"

#include "Utils/string_algorithms.hpp"

#include "Gameplay/Magic/Sources/SourceTarget.hpp"

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
		saveSection(str::trim_whitespace(_savePicker->getText()));
		C::game->exitScreen();
		return;
	}

	if (IM::isKeyJustPressed(sf::Keyboard::Left)) {
		if (_toolState == place_source) sourceSwitcher_->left();
		if (_toolState == place_ennemy) ennemySwitcher_->left();
	}

	if (IM::isKeyJustPressed(sf::Keyboard::Right)) {
		if (_toolState == place_source) sourceSwitcher_->right();
		if (_toolState == place_ennemy) ennemySwitcher_->right();
	}

	if (IM::isLastSequenceJustFinished({ sf::Keyboard::LControl, sf::Keyboard::D }) ||
		IM::isMousePressed(sf::Mouse::Right)) {
		deleteHovered();
	}

	if (IM::isLastSequenceJustFinished({ sf::Keyboard::LShift, sf::Keyboard::Add })) {
		_snapLevel *= 10.f;
	}
	else if (
		IM::isLastSequenceJustFinished({ sf::Keyboard::LShift, sf::Keyboard::Subtract })
	) {
		_snapLevel /= 10.f;
	}

	_snapLevel = std::clamp(_snapLevel, 0.0001f, 1.f);

	_snapGrid->setStdString("Snap grid " + std::to_string(_snapLevel) + "m");

	inputSwitchState();
	_widgets.at("root")->propagateInput();

	switch (_toolState)
	{
	case draw_plateforme:
		updateDrawPlateforme();
		break;
	case place_ennemy:
		updatePlaceSlime();
		break;
	case place_start_pos:
		updatePlaceStartPos();
		break;
	case nothing:
		updateNothing();
		break;
	case place_source:
		updateSource();
	default:
		break;
	}

	updateCameraMovement(dt);
}

void EditSectionScreen::updateSource() noexcept {
	_newSource->pos = getSnapedMouseCameraPos();
	if (IM::isMouseJustPressed(sf::Mouse::Left)) {
		_newSource->sprite = sourceSwitcher_->getCurrentPanel()->getTexture();

		if (sourceSwitcher_->getCurrentPanel()->getName() == "source") {
			_section.sources.push_back(*_newSource);
		}
		else if (sourceSwitcher_->getCurrentPanel()->getName() == "source_boomerang") {
			auto info = 
				SourceTargetInfo::loadJson(AM::getJson(SourceTargetInfo::JSON_ID));
			info.source.pos = getSnapedMouseCameraPos();

			_section.sourcesBoomerang.push_back(info);
		}

	}
}
void EditSectionScreen::updatePlaceSlime() noexcept {
	if (IM::isMouseJustPressed(sf::Mouse::Left)) {
		if (ennemySwitcher_->getCurrentPanel()->getName() == SlimeInfo::JSON_ID) {
			auto info = SlimeInfo::loadJson(AM::getJson(SlimeInfo::JSON_ID));
			info.startPos = getSnapedMouseCameraPos();
			_section.slimes.push_back(info);
		}
		else if (
			ennemySwitcher_->getCurrentPanel()->getName() == DistanceGuyInfo::JSON_ID
		) {
			auto info = DistanceGuyInfo::loadJson(AM::getJson(DistanceGuyInfo::JSON_ID));
			info.startPos = getSnapedMouseCameraPos();
			_section.distanceGuys.push_back(info);
		}
	}
}
void EditSectionScreen::updatePlaceStartPos() noexcept {
	if (IM::isMouseJustPressed(sf::Mouse::Left)) {
		_section.startPos = getSnapedMouseCameraPos();
		exitToolState();
	}
}
void EditSectionScreen::updateDrawPlateforme() noexcept {
	if (IM::isMouseJustPressed(sf::Mouse::Left)) {
		switch (_toolState) {
		case draw_plateforme:
			auto p = PlateformeInfo();
			p.rectangle.pos = getSnapedMouseCameraPos();

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
			getSnapedMouseCameraPos() - _newPlateforme->rectangle.pos;
	}
}

void EditSectionScreen::updateNothing() noexcept {
	
}

void EditSectionScreen::updateCameraMovement(double dt) noexcept {
	if (IM::isKeyPressed(sf::Keyboard::LControl)) return;

	float speedFactor = IM::isKeyPressed(sf::Keyboard::LShift) ? 3.f : 1.f;

	if (IM::isKeyPressed(sf::Keyboard::Z)) {
		_cameraView.move({ 0.f, -(float)(dt * _section.maxRectangle.h / 3.f * speedFactor) });
	}
	if (IM::isKeyPressed(sf::Keyboard::Q)) {
		_cameraView.move({ -(float)(dt * _section.maxRectangle.w / 3.f) * speedFactor , 0.f });
	}
	if (IM::isKeyPressed(sf::Keyboard::S)) {
		_cameraView.move({ 0.f, +(float)(dt * _section.maxRectangle.h / 3.f) * speedFactor });
	}
	if (IM::isKeyPressed(sf::Keyboard::D)) {
		_cameraView.move({ +(float)(dt * _section.maxRectangle.w / 3.f) * speedFactor , 0.f});
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
		if (_toolState != place_ennemy) {
			exitToolState();
			enterToolState(place_ennemy);
		}
		else {
			exitToolState();
		}
	}
	else if (IM::isLastSequenceJustFinished({
		sf::Keyboard::LControl, sf::Keyboard::S, sf::Keyboard::S
	})){
		if (_toolState != place_start_pos) {
			exitToolState();
			enterToolState(place_start_pos);
		}
		else {
			exitToolState();
		}
	}
	else if (IM::isLastSequenceJustFinished({
		sf::Keyboard::LControl, sf::Keyboard::S, sf::Keyboard::O
	})) {
		if (_toolState != place_source) {
			exitToolState();
			enterToolState(place_source);
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
	for (auto distance : _section.distanceGuys) {
		renderDebug(target, distance);
	}
	for (auto source : _section.sources) {
		renderDebug(target, source);
	}
	for (auto source : _section.sourcesBoomerang) {
		renderDebug(target, source.source);
	}

	if (_newPlateforme.has_value()) {
		renderDebug(target, *_newPlateforme);
	}
	if (_toolState == place_ennemy) {
		getSnapedMouseCameraPos().plot(target, 0.1f, { 1.0, 0.0, 0.0, 1.0 }, {}, 0.f);
	}
	if (_newSource.has_value()) {
		renderDebug(target, *_newSource);
	}

	Vector2f startPos = _section.startPos;
	if (_toolState == place_start_pos) {
		startPos = getSnapedMouseCameraPos();
	}
	startPos.plot(target, 0.1f, { 1.0, 0.0, 0.0, 1.0 }, {}, 0.f);

	target.setView(_uiView);
	_widgets.at("root")->propagateRender(target);

	target.setView(old);
}

void EditSectionScreen::onEnter(std::any) {
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

	ennemySwitcher_ = (SpriteSwitcher*)(_widgets.at("root")->findChild("ennemySwitcher"));
	sourceSwitcher_ = (SpriteSwitcher*)(_widgets.at("root")->findChild("sourceSwitcher"));
	_savePicker = (ValuePicker*)(_widgets.at("root")->findChild("savePicker"));
	_snapGrid = (Label*)(_widgets.at("root")->findChild("snapGrid"));
}

std::any EditSectionScreen::onExit() {
	for (auto&[key, value] : _widgets) {
		delete value;
	}
	return { _section };
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
	case place_ennemy:
		ennemySwitcher_->setVisible(true);
		changeColorLabel(PLACE_SLIME, { 0.0, 1.0, 0.0, 1.0 });
		break;
	case place_start_pos:
		changeColorLabel(PLACE_START_POS, { 0.0, 1.0, 0.0, 1.0 });
		break;
	case place_source:
		_newSource = SourceInfo();
		changeColorLabel(PLACE_SOURCE, { 0.0, 1.0, 0.0, 1.0 }); 
		sourceSwitcher_->setVisible(true);
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
	case place_ennemy:
		ennemySwitcher_->setVisible(false);
		changeColorLabel(PLACE_SLIME, { 1.0, 1.0, 1.0, 1.0 });
		break;
	case place_start_pos:
		changeColorLabel(PLACE_START_POS, { 1.0, 1.0, 1.0, 1.0 });
		break;
	case place_source:
		_newSource.reset();
		changeColorLabel(PLACE_SOURCE, { 1.0, 1.0, 1.0, 1.0 });
		sourceSwitcher_->setVisible(false);
		break;
	default:
		break;
	}
	_toolState = nothing;
}

void EditSectionScreen::renderDebug(
	sf::RenderTarget& target, PlateformeInfo info
) noexcept {
	Vector4f color{ 0.8f, 0.8f, 0.0f, 1.0f };
	if (info.rectangle.in(IM::getMousePosInView(_cameraView))) {
		color = { 1.0f, 0.7f, 0.0f, 1.0f };
	}
	info.rectangle.render(target, color);
}
void EditSectionScreen::renderDebug(
	sf::RenderTarget& target, SlimeInfo info
) noexcept {
	Vector4d color{ 0.0, 0.8, 0.8, 1.0 };
	auto dist2 = (info.startPos - IM::getMousePosInView(_cameraView)).length2();
	if (dist2 < info.size.x * info.size.x / 4.f) {
		color = { 0.0, 0.7, 1.0, 1.0 };
	}
	info.startPos.plot(
		target, info.size.x / 2.f, color, { 0.0, 0.0, 0.0, 0.0 }, 0.f
	);
}
void EditSectionScreen::renderDebug(
	sf::RenderTarget& target, DistanceGuyInfo info
) noexcept {
	Vector4d color{ 0.1, 0.7, 0.7, 1.0 };
	auto dist2 = (info.startPos - IM::getMousePosInView(_cameraView)).length2();
	if (dist2 < info.size.x * info.size.x / 4.f) {
		color = { 0.0, 0.6, 0.9, 1.0 };
	}
	info.startPos.plot(
		target, info.size.x / 2.f, color, { 0.0, 0.0, 0.0, 0.0 }, 0.f
	);
}
void EditSectionScreen::renderDebug(
	sf::RenderTarget& target, SourceInfo info
) noexcept {
	Vector4d color{ 0.8, 0.0, 0.8, 1.0 };
	auto dist2 = (info.pos - IM::getMousePosInView(_cameraView)).length2();
	if (dist2 < info.size.x * info.size.x / 4.f) {
		color = { 0.7, 0.0, 1.0, 1.0 };
	}
	info.pos.plot(
		target, info.size.x / 2.f, color, { 0.0, 0.0, 0.0, 0.0 }, 0.f
	);
}

void EditSectionScreen::changeColorLabel(std::string name, Vector4f color) noexcept {
	auto root = _widgets.at("root");

	if (auto it = root->findChild(name); dynamic_cast<const Label*>(it)) {
		((Label*)it)->setTextColor(color);
	}
	else {
		std::abort();
	}
}

void EditSectionScreen::saveSection(std::string path) const noexcept {
	auto json = SectionInfo::saveJson(_section);
	std::ofstream file;
	file.open(ASSETS_PATH + path);
	
	static char buffer[512];
	strerror_s(buffer, errno);

	if (!file.is_open()) {
		printf("Error on saving the file %s, %s\n", path.c_str(), buffer);
	}

	file << json;

	file.close();
}

void EditSectionScreen::deleteHovered() noexcept {
	auto& plateformes = _section.plateformes;
	auto& slimes = _section.slimes;
	auto& distance = _section.distanceGuys;
	auto& sources = _section.sources;
	auto& sourcesBoomerang = _section.sourcesBoomerang;

	for (size_t i = plateformes.size(); i > 0; --i) {
		if (plateformes[i - 1].rectangle.in(IM::getMousePosInView(_cameraView))) {
			plateformes.erase(std::begin(plateformes) + i - 1);
			return;
		}
	}

	for (size_t i = slimes.size(); i > 0; --i) {

		auto dist2 = (slimes[i - 1].startPos - IM::getMousePosInView(_cameraView)).length2();
		if (dist2 < slimes[i - 1].size.x * slimes[i - 1].size.x / 4.f) {
			slimes.erase(std::begin(slimes) + i - 1);
			return;
		}
	}
	for (size_t i = distance.size(); i > 0; --i) {

		auto dist2 = (distance[i - 1].startPos - IM::getMousePosInView(_cameraView))
			.length2();
		if (dist2 < distance[i - 1].size.x * distance[i - 1].size.x / 4.f) {
			distance.erase(std::begin(distance) + i - 1);
			return;
		}
	}

	for (size_t i = sources.size(); i > 0; --i) {

		auto dist2 = (sources[i - 1].pos - IM::getMousePosInView(_cameraView)).length2();
		if (dist2 < sources[i - 1].size.x * sources[i - 1].size.x / 4.f) {
			sources.erase(std::begin(sources) + i - 1);
			return;
		}
	}

	for (size_t i = sourcesBoomerang.size(); i > 0; --i) {

		auto dist2 = 
			(sourcesBoomerang[i - 1].source.pos - IM::getMousePosInView(_cameraView))
			.length2();

		if (
			dist2 < sourcesBoomerang[i - 1].source.size.x *
			sourcesBoomerang[i - 1].source.size.x / 4.f
		) {
			sourcesBoomerang.erase(std::begin(sourcesBoomerang) + i - 1);
			return;
		}
	}
}

Vector2f EditSectionScreen::getSnapedMouseScreenPos() const noexcept {
	auto pos = IM::getMouseScreenPos();

	return {
		(unsigned long)(pos.x / _snapLevel) * _snapLevel,
		(unsigned long)(pos.y / _snapLevel) * _snapLevel
	};
}

Vector2f EditSectionScreen::getSnapedMouseCameraPos() const noexcept {
	auto pos = IM::getMousePosInView(_cameraView);

	return {
		(long long)(pos.x / _snapLevel - 0.5f) * _snapLevel,
		(long long)(pos.y / _snapLevel - 0.5f) * _snapLevel
	};
}