#include "EditSectionScreen.hpp"

#include <fstream>
#include <algorithm>
#include <filesystem>

#include "Managers/InputsManager.hpp"
#include "Managers/AssetsManager.hpp"

#include "Game.hpp"

#include "LevelScreen.hpp"

#include "Math/algorithms.hpp"

#include "Graphics/GUI/Switcher.hpp"
#include "Graphics/GUI/Label.hpp"
#include "Graphics/GUI/ValuePicker.hpp"

#include "Utils/string_algorithms.hpp"

#include "Gameplay/Magic/Sources/SourceTarget.hpp"
#include "Gameplay/Magic/Sources/SourceVaccum.hpp"
#include "Gameplay/Magic/Sources/SourceDirection.hpp"

#include "Gameplay/Characters/Fly.hpp"

#include "OS/OpenFile.hpp"

EditSectionScreen::EditSectionScreen(Section* section) :
	Screen(),
	sectionInfo_(section->getInfo()),
	filepath_(section->getFilepath()),
	_uiView({ WIDTH / 2.f, HEIGHT / 2.f }, { (float)WIDTH, (float)HEIGHT })
{
	_cameraView = section->getCameraView();

	viewSize_.size = sectionInfo_.viewSize.fitUpRatio(RATIO);
}

void EditSectionScreen::update(double dt) {
	if (toLoadFile) {
		std::lock_guard{ fileToLoadMutex };
		loadSectionFile(fileToLoad);
	}

	bool shiftPressed =
		IM::isKeyPressed(sf::Keyboard::LShift) || IM::isKeyPressed(sf::Keyboard::RShift);

	if (IM::isKeyPressed(sf::Keyboard::LControl) &&
		IM::isKeyJustPressed(sf::Keyboard::E)
	) {
		saveSection(str::trim_whitespace(_savePicker->getStdString()));
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

	if (IM::isLastSequenceJustFinished({
		sf::Keyboard::LControl, sf::Keyboard::T, sf::Keyboard::V
	})) {
		viewSize_.pos = _cameraView.getCenter() - _cameraView.getSize() / 2.f;
		viewSize_.size = _cameraView.getSize();
		sectionInfo_.viewSize = viewSize_.size;
	}

	if (IM::isLastSequenceJustFinished({ sf::Keyboard::LControl, sf::Keyboard::D }) ||
		IM::isMousePressed(sf::Mouse::Right)) {
		deleteHovered();
	}

	if (shiftPressed && IM::isKeyJustPressed(sf::Keyboard::Add)) {
		_snapLevel *= 2.f;
	}
	else if (shiftPressed && IM::isKeyJustPressed(sf::Keyboard::Subtract)) {
		_snapLevel /= 2.f;
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
		break;
	case place_navigation_point:
		updatePlaceNavigationPoint();
		break;
	case place_navigation_link:
		updatePlaceNavigationLink();
		break;
	default:
		break;
	}

	updateCameraMovement(dt);
}

void EditSectionScreen::updatePlaceNavigationLink() noexcept {
	if (IM::isMouseJustPressed(sf::Mouse::Left)) {
		for (auto point : sectionInfo_.navigationPoints) {
			if (Vector2f::equal(point.pos, getSnapedMouseCameraPos(), point.range)) {
				if (firstPoint_ && *firstPoint_ == point.id) {
					firstPoint_.reset();
					return;
				}

				if (!firstPoint_) {
					firstPoint_ = point.id;
					return;
				}

				NavigationLinkInfo info;
				info.A = *firstPoint_;
				info.B = point.id;
				info.id = UUID();

				sectionInfo_.navigationLinks.push_back(info);

				auto A = std::find_if(
					std::begin(sectionInfo_.navigationPoints),
					std::end(sectionInfo_.navigationPoints),
					[X = *firstPoint_](auto x) {return x.id == X; }
				);
				auto B = std::find_if(
					std::begin(sectionInfo_.navigationPoints),
					std::end(sectionInfo_.navigationPoints),
					[X = point.id](auto x) {return x.id == X; }
				);

				A->links.push_back(info.id);
				B->links.push_back(info.id);

				firstPoint_.reset();
			}
		}
	}
}
void EditSectionScreen::updatePlaceNavigationPoint() noexcept {
	if (IM::isMouseJustPressed(sf::Mouse::Left)) {
		NavigationPointInfo info;
		info.pos = getSnapedMouseCameraPos();
		info.id = UUID();
		info.range = 0.3f;

		sectionInfo_.navigationPoints.push_back(info);
	}
}
void EditSectionScreen::updateSource() noexcept {
	_newSource->pos = getSnapedMouseCameraPos();
	if (IM::isMouseJustPressed(sf::Mouse::Left)) {
		_newSource->sprite = sourceSwitcher_->getCurrentPanel()->getTexture();

		if (sourceSwitcher_->getCurrentPanel()->getName() == "source") {
			sectionInfo_.sources.push_back(*_newSource);
		}
		else if (sourceSwitcher_->getCurrentPanel()->getName() == SourceTarget::JSON_ID) {
			auto info =
				SourceTargetInfo::loadJson(AM::getJson(SourceTargetInfo::JSON_ID));
			info.source.pos = getSnapedMouseCameraPos();

			sectionInfo_.sourcesBoomerang.push_back(info);
		}
		else if (sourceSwitcher_->getCurrentPanel()->getName() == SourceVaccum::JSON_ID) {
			auto info =
				SourceVaccumInfo::loadJson(AM::getJson(SourceVaccum::JSON_ID));
			info.source.pos = getSnapedMouseCameraPos();

			sectionInfo_.sourcesVaccum.push_back(info);
		}
		else if (
			sourceSwitcher_->getCurrentPanel()->getName() == SourceDirection::JSON_ID
		) {
			auto info =
				SourceDirectionInfo::loadJson(AM::getJson(SourceDirectionInfo::JSON_ID));
			info.source.pos = getSnapedMouseCameraPos();

			sectionInfo_.sourcesDirection.push_back(info);
		}

	}
}
void EditSectionScreen::updatePlaceSlime() noexcept {
	if (IM::isMouseJustPressed(sf::Mouse::Left)) {
		if (ennemySwitcher_->getCurrentPanel()->getName() == SlimeInfo::JSON_ID) {
			auto info = SlimeInfo::loadJson(AM::getJson(SlimeInfo::JSON_ID));
			info.startPos = getSnapedMouseCameraPos();
			sectionInfo_.slimes.push_back(info);
		}
		else if (
			ennemySwitcher_->getCurrentPanel()->getName() == DistanceGuyInfo::JSON_ID
		) {
			auto info = DistanceGuyInfo::loadJson(AM::getJson(DistanceGuyInfo::JSON_ID));
			info.startPos = getSnapedMouseCameraPos();
			sectionInfo_.distanceGuys.push_back(info);
		}
		else if (
			ennemySwitcher_->getCurrentPanel()->getName() == MeleeGuyInfo::JSON_ID
		) {
			auto info = MeleeGuyInfo::loadJson(AM::getJson(MeleeGuyInfo::JSON_ID));
			info.startPos = getSnapedMouseCameraPos();
			sectionInfo_.meleeGuys.push_back(info);
		}
		else if (
			ennemySwitcher_->getCurrentPanel()->getName() == FlyInfo::JSON_ID
		) {
			auto info = FlyInfo::loadJson(AM::getJson(FlyInfo::JSON_ID));
			info.startPos = getSnapedMouseCameraPos();
			sectionInfo_.flies.push_back(info);
		}
	}
}
void EditSectionScreen::updatePlaceStartPos() noexcept {
	if (IM::isMouseJustPressed(sf::Mouse::Left)) {
		sectionInfo_.startPos = getSnapedMouseCameraPos();
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

			sectionInfo_.plateformes.push_back(_newPlateforme.value());
			std::vector<Rectangle2f> rectangles;
			std::transform(
				std::begin(sectionInfo_.plateformes),
				std::end(sectionInfo_.plateformes),
				std::back_inserter(rectangles),
				[](PlateformeInfo info) {
					return info.rectangle;
				}
			);
			sectionInfo_.maxRectangle = Rectangle2f::hull(rectangles);
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
		_cameraView.move({ 0.f, -(float)(dt * sectionInfo_.maxRectangle.h / 3.f * speedFactor) });
	}
	if (IM::isKeyPressed(sf::Keyboard::Q)) {
		_cameraView.move({ -(float)(dt * sectionInfo_.maxRectangle.w / 3.f) * speedFactor , 0.f });
	}
	if (IM::isKeyPressed(sf::Keyboard::S)) {
		_cameraView.move({ 0.f, +(float)(dt * sectionInfo_.maxRectangle.h / 3.f) * speedFactor });
	}
	if (IM::isKeyPressed(sf::Keyboard::D)) {
		_cameraView.move({ +(float)(dt * sectionInfo_.maxRectangle.w / 3.f) * speedFactor , 0.f});
	}
	if (float delta = IM::getLastScroll(); delta != 0.f) {
		_cameraView.zoom(std::powf(1.2f, -delta));
	}
}

void EditSectionScreen::inputSwitchState() noexcept {
	if (IM::isLastSequenceJustFinished({
			sf::Keyboard::LControl, sf::Keyboard::Q, sf::Keyboard::P
	})){
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
	})) {
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
	else if (
		IM::isKeyPressed(sf::Keyboard::LControl) && IM::isKeyJustPressed(sf::Keyboard::N)
	) {
		if (_toolState != place_navigation_point) {
			exitToolState();
			enterToolState(place_navigation_point);
		}
		else {
			exitToolState();
		}
	}
	else if (
		IM::isKeyPressed(sf::Keyboard::LControl) && IM::isKeyJustPressed(sf::Keyboard::L)
	) {
		if (_toolState != place_navigation_link) {
			exitToolState();
			enterToolState(place_navigation_link);
		}
		else {
			exitToolState();
		}
	}
	else if (IM::isLastSequenceJustFinished({
		sf::Keyboard::LControl, sf::Keyboard::O, sf::Keyboard::S
	})) {
		open_file_async([&](OpenFileResult file) {
			// i don't know if it's usefull to prevent race condition.
			if (this == nullptr) return;
			if (!file.succeded) return;

			std::lock_guard{ fileToLoadMutex };
			fileToLoad = file.filepath;
			toLoadFile = true;
		});
	}
	else if (IM::isKeyJustPressed(sf::Keyboard::Escape) && _toolState != nothing) {
		exitToolState();
	}
}

void EditSectionScreen::render(sf::RenderTarget& target) {
	auto old = target.getView();
	
	target.setView(_cameraView);

	viewSize_.pos = (Vector2f)_cameraView.getCenter() - viewSize_.size / 2.f;

	for (auto plateforme : sectionInfo_.plateformes) {
		renderDebug(target, plateforme);
	}
	for (auto slime : sectionInfo_.slimes) {
		renderDebug(target, slime);
	}
	for (auto distance : sectionInfo_.distanceGuys) {
		renderDebug(target, distance);
	}
	for (auto melee : sectionInfo_.meleeGuys) {
		renderDebug(target, melee);
	}
	for (auto fly : sectionInfo_.flies) {
		renderDebug(target, fly);
	}
	for (auto source : sectionInfo_.sources) {
		renderDebug(target, source);
	}
	for (auto source : sectionInfo_.sourcesBoomerang) {
		renderDebug(target, source.source);
	}
	for (auto source : sectionInfo_.sourcesVaccum) {
		renderDebug(target, source.source);
	}
	for (auto source : sectionInfo_.sourcesDirection) {
		renderDebug(target, source.source);
	}
	for (auto p : sectionInfo_.navigationPoints) {
		renderDebug(target, p);
	}
	for (auto p : sectionInfo_.navigationLinks) {
		renderDebug(target, p);
	}

	if (_newPlateforme.has_value()) {
		renderDebug(target, *_newPlateforme);
	}
	if (_toolState == place_ennemy) {
		getSnapedMouseCameraPos().plot(target, 0.1f, { 1.0, 0.0, 0.0, 1.0 }, {}, 0.f);
	}
	if (_toolState == place_navigation_point) {
		getSnapedMouseCameraPos().plot(target, 0.15f, { 1.0, 0.0, 0.0, 1.0 }, {}, 0.f);
	}
	if (_toolState == place_navigation_link && firstPoint_) {
		auto point = std::find_if(
			std::begin(sectionInfo_.navigationPoints),
			std::end(sectionInfo_.navigationPoints),
			[id = *firstPoint_](auto x) {return x.id == id; }
		);

		assert(point != std::end(sectionInfo_.navigationPoints));

		point->pos.plot(
			target, point->range * 1.2f, { 1.0, 1.0, 0.0, 1.0 }, {1.0, 1.0, 1.0, 1.0}, 0.1f
		);
	}
	if (_newSource.has_value()) {
		renderDebug(target, *_newSource);
	}

	Vector2f startPos = sectionInfo_.startPos;
	if (_toolState == place_start_pos) {
		startPos = getSnapedMouseCameraPos();
	}
	startPos.plot(target, 0.1f, { 1.0, 0.0, 0.0, 1.0 }, {}, 0.f);

	viewSize_.render(
		target, Vector4f{ 0.f, 0.f, 0.f, 0.f }, Vector4f{ 1.f, 0.f, 0.f, 1.f }
	);

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
	_savePicker->setStdString(filepath_.generic_string());
	_snapGrid = (Label*)(_widgets.at("root")->findChild("snapGrid"));
}

std::any EditSectionScreen::onExit() {
	filepath_ = _savePicker->getStdString();
	for (auto&[key, value] : _widgets) {
		delete value;
	}
	return ReturnType{sectionInfo_, filepath_};
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
	case place_navigation_point:
		changeColorLabel(PLACE_NAVIGATION_POINT, { 0.0, 1.0, 0.0, 1.0 });
		break;
	case place_navigation_link:
		changeColorLabel(PLACE_NAVIGATION_LINK, { 0.0, 1.0, 0.0, 1.0 });
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
	case place_navigation_point:
		changeColorLabel(PLACE_NAVIGATION_POINT, { 1.0, 1.0, 1.0, 1.0 });
		break;
	case place_navigation_link:
		changeColorLabel(PLACE_NAVIGATION_LINK, { 1.0, 1.0, 1.0, 1.0 });
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
	sf::RenderTarget& target, MeleeGuyInfo info
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
	sf::RenderTarget& target, FlyInfo info
) noexcept {
	Vector4d color{ 0.1, 0.7, 0.7, 1.0 };
	auto dist2 = (info.startPos - IM::getMousePosInView(_cameraView)).length2();
	if (dist2 < info.radius * info.radius / 4.f) {
		color = { 0.0, 0.6, 0.9, 1.0 };
	}
	info.startPos.plot(
		target, info.radius / 2.f, color, { 0.0, 0.0, 0.0, 0.0 }, 0.f
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
void EditSectionScreen::renderDebug(
	sf::RenderTarget& target, NavigationPointInfo info
) noexcept {
	Vector4d color{ 0.8, 0.0, 0.8, 1.0 };
	auto dist2 = (info.pos - IM::getMousePosInView(_cameraView)).length2();
	if (dist2 < info.range * info.range) {
		color = { 0.7, 0.0, 1.0, 1.0 };
	}
	info.pos.plot(target, info.range, color, { 0.0, 0.0, 0.0, 0.0 }, 0.f);
}
void EditSectionScreen::renderDebug(
	sf::RenderTarget& target, NavigationLinkInfo info
) noexcept {
	Vector4d color{ 0.8, 0.0, 0.8, 1.0 };

	auto a = std::find_if(
		std::begin(sectionInfo_.navigationPoints),
		std::end(sectionInfo_.navigationPoints),
		[a = info.A](auto x) {return x.id == a; }
	);
	auto b = std::find_if(
		std::begin(sectionInfo_.navigationPoints),
		std::end(sectionInfo_.navigationPoints),
		[b = info.B](auto x) {return x.id == b; }
	);

	assert(a != b);
	assert(a != std::end(sectionInfo_.navigationPoints));
	assert(b != std::end(sectionInfo_.navigationPoints));

	if (is_in_ellipse(a->pos, b->pos, 10, IM::getMouseScreenPos())) {
		color = { 0.7, 0.0, 1.0, 1.0 };
	}

	Vector2f::renderLine(target, a->pos, b->pos, color);
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

void EditSectionScreen::saveSection(std::filesystem::path path) const noexcept {
	auto json = SectionInfo::saveJson(sectionInfo_);

	std::filesystem::current_path(EXE_DIR);
	std::ofstream file;
	file.open(ASSETS_PATH / path);
	
	static char buffer[512];
	strerror_s(buffer, errno);

	if (!file.is_open()) {
		printf(
			"Error on saving the file %s, %s\n",
			(ASSETS_PATH / path).string().c_str(),
			buffer
		);
	}

	file << json;

	file.close();
}

void EditSectionScreen::deleteHovered() noexcept {
	auto& plateformes = sectionInfo_.plateformes;
	auto& slimes = sectionInfo_.slimes;
	auto& distance = sectionInfo_.distanceGuys;
	auto& meleeGuy = sectionInfo_.meleeGuys;
	auto& flies = sectionInfo_.flies;
	auto& sources = sectionInfo_.sources;
	auto& sourcesBoomerang = sectionInfo_.sourcesBoomerang;
	auto& sourcesDirection = sectionInfo_.sourcesDirection;
	auto& sourcesVaccum = sectionInfo_.sourcesVaccum;
	auto& navigationPoints = sectionInfo_.navigationPoints;
	auto& navigationLinks = sectionInfo_.navigationLinks;

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
	for (size_t i = meleeGuy.size(); i > 0; --i) {
		auto dist2 = (meleeGuy[i - 1].startPos - IM::getMousePosInView(_cameraView))
			.length2();
		if (dist2 < meleeGuy[i - 1].size.x * meleeGuy[i - 1].size.x / 4.f) {
			meleeGuy.erase(std::begin(meleeGuy) + i - 1);
			return;
		}
	}
	for (size_t i = flies.size(); i > 0; --i) {
		auto dist2 = (flies[i - 1].startPos - IM::getMousePosInView(_cameraView))
			.length2();
		if (dist2 < flies[i - 1].radius * flies[i - 1].radius / 4.f) {
			flies.erase(std::begin(flies) + i - 1);
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
	for (size_t i = sourcesVaccum.size(); i > 0; --i) {

		auto dist2 =
			(sourcesVaccum[i - 1].source.pos - IM::getMousePosInView(_cameraView))
			.length2();

		if (
			dist2 < sourcesVaccum[i - 1].source.size.x *
			sourcesVaccum[i - 1].source.size.x / 4.f
			) {
			sourcesVaccum.erase(std::begin(sourcesVaccum) + i - 1);
			return;
		}
	}
	for (size_t i = sourcesDirection.size(); i > 0; --i) {

		auto dist2 =
			(sourcesDirection[i - 1].source.pos - IM::getMousePosInView(_cameraView))
			.length2();

		if (
			dist2 < sourcesDirection[i - 1].source.size.x *
			sourcesDirection[i - 1].source.size.x / 4.f
		) {
			sourcesDirection.erase(std::begin(sourcesDirection) + i - 1);
			return;
		}
	}
	for (size_t i = navigationPoints.size(); i > 0; --i) {
		auto dist2 =
			(navigationPoints[i - 1].pos - IM::getMousePosInView(_cameraView))
			.length2();

		if (dist2 < navigationPoints[i - 1].range * navigationPoints[i - 1].range) {

			auto navToErase = navigationPoints[i - 1].id;

			navigationLinks.erase(std::remove_if(
				std::begin(navigationLinks),
				std::end(navigationLinks),
				[navToErase](auto x) {
					return x.A == navToErase || x.B == navToErase;
				}
			), std::end(navigationLinks));

			navigationPoints.erase(std::begin(navigationPoints) + i - 1);
			return;
		}
	}
	for (size_t i = navigationLinks.size(); i > 0; --i) {
		auto link = navigationLinks[i - 1];

		auto a = std::find_if(
			std::begin(sectionInfo_.navigationPoints),
			std::end(sectionInfo_.navigationPoints),
			[a = link.A](auto x) {return x.id == a; }
		);
		auto b = std::find_if(
			std::begin(sectionInfo_.navigationPoints),
			std::end(sectionInfo_.navigationPoints),
			[b = link.B](auto x) {return x.id == b; }
		);

		assert(a != b);
		assert(a != std::end(sectionInfo_.navigationPoints));
		assert(b != std::end(sectionInfo_.navigationPoints));

		if (is_in_ellipse(a->pos, b->pos, 10, IM::getMouseScreenPos())) {
			a->links.erase(
				std::remove_if(
					std::begin(a->links),
					std::end(a->links),
					[X = link.id](auto x) {return x == X; }
				),
				std::end(a->links)
			);
			
			b->links.erase(
				std::remove_if(
					std::begin(b->links),
					std::end(b->links),
					[X = link.id](auto x) {return x == X; }
				),
				std::end(b->links)
			);

			navigationLinks.erase(std::begin(navigationLinks) + i - 1);
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

// you need to save manually.
void EditSectionScreen::loadSectionFile(std::filesystem::path path) noexcept {
	if (!AM::loadJson(path.generic_string(), path.generic_string())) {
		printf("Couldn't load %s.", path.generic_string().c_str());
		return;
	}

	sectionInfo_ = SectionInfo::loadJson(AM::getJson(path.generic_string()));

	namespace fs = std::filesystem;


	fs::path path_os{ path };
	auto relative_path = fs::relative(path_os, EXE_DIR / ASSETS_PATH).generic_string();
	_savePicker->setStdString(relative_path);

	toLoadFile = false;
	fileToLoad.clear();
}