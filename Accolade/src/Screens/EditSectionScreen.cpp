#include "EditSectionScreen.hpp"

#include <fstream>
#include <algorithm>
#include <filesystem>

#include "Utils/meta_algorithms.hpp"

#include "Managers/InputsManager.hpp"
#include "Managers/AssetsManager.hpp"

#include "Game.hpp"

#include "LevelScreen.hpp"

#include "Math/algorithms.hpp"

#include "Graphics/GUI/Switcher.hpp"
#include "Graphics/GUI/Label.hpp"
#include "Graphics/GUI/Panel.hpp"
#include "Graphics/GUI/Button.hpp"
#include "Graphics/GUI/ValuePicker.hpp"
#include "Graphics/GUI/PosPicker.hpp"
#include "Graphics/GUI/algorithm.hpp"
#include "Graphics/GUI/JsonTree.hpp"
#include "Graphics/GUI/dyn_structTree.hpp"

#include "Utils/string_algorithms.hpp"

#include "Gameplay/Magic/Sources/SourceTarget.hpp"
#include "Gameplay/Magic/Sources/SourceVaccum.hpp"
#include "Gameplay/Magic/Sources/SourceDirection.hpp"

#include "Gameplay/Characters/Fly.hpp"

#include "OS/OpenFile.hpp"

static constexpr auto root_properties_name = "properties";

EditSectionScreen::EditSectionScreen(Section* section) :
	Screen(),
	sectionInfo_(section->getInfo()),
	filepath_(section->getFilepath()),
	_uiView({ WIDTH / 2.f, HEIGHT / 2.f }, { (float)WIDTH, (float)HEIGHT })
{
	cameraView = (OwnId<sf::View>)es_instance->copy(section->getCameraView());

	viewSize_.size = sectionInfo_.viewSize.fitUpRatio(RATIO);
}

void EditSectionScreen::input(double dt) noexcept {
	input_caught_by_ui.reset();
	for (auto&[_, w] : _widgets) {
		_;
		input_caught_by_ui |= w->postOrderInput(input_caught_by_ui);
	}

	if (input_caught_by_ui.test(Widget::Input_Mask::Key_Ended)) return;
	updateCameraMovement(dt);
	inputSwitchState();

	bool shiftPressed =
		IM::isKeyPressed(sf::Keyboard::LShift) || IM::isKeyPressed(sf::Keyboard::RShift);

	if (
		IM::isKeyPressed(sf::Keyboard::LControl) &&
		IM::isKeyJustPressed(sf::Keyboard::E)
	) {
		saveSection(str::trim_whitespace(_savePicker->getStdString()));
		go_back_screen = true;
		return;
	}

	if (IM::isKeyJustPressed(sf::Keyboard::Left)) {
		if (_toolState == place_source) sourceSwitcher_->left();
		if (_toolState == place_ennemy) ennemySwitcher_->left();
		if (_toolState == place_structure) structureSwitcher->left();
	}

	if (IM::isKeyJustPressed(sf::Keyboard::Right)) {
		if (_toolState == place_source) sourceSwitcher_->right();
		if (_toolState == place_ennemy) ennemySwitcher_->right();
		if (_toolState == place_structure) structureSwitcher->right();
	}

	if (IM::isLastSequenceJustFinished({
		sf::Keyboard::LControl, sf::Keyboard::T, sf::Keyboard::V
		})) {
		viewSize_.pos =
			es_instance->get(cameraView)->getCenter() -
			es_instance->get(cameraView)->getSize() / 2.f;
		viewSize_.size = es_instance->get(cameraView)->getSize();
		sectionInfo_.viewSize = viewSize_.size;
	}

	if (
		IM::isKeyPressed(sf::Keyboard::LControl) &&
		(IM::isLastSequenceJustFinished({ sf::Keyboard::D }) ||
			IM::isMousePressed(sf::Mouse::Right))
		) {
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
}

void EditSectionScreen::update(double dt) {
	input(dt);

	if (toLoadFile) {
		std::lock_guard{ fileToLoadMutex };
		loadSectionFile(fileToLoad);
	}

	switch (_toolState)
	{
	case place_structure:
		updatePlaceStructure();
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
		_newSource->texture = sourceSwitcher_->getCurrentPanel()->getTexture();

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
	// >Add ennemy.
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
		else if (
			ennemySwitcher_->getCurrentPanel()->getName() == FirstBossInfo::JSON_ID
		) {
			FirstBossInfo info;
			from_json(AM::getJson(FirstBossInfo::JSON_ID), info);
			info.start_pos = getSnapedMouseCameraPos();
			sectionInfo_.first_bosses.push_back(info);
		}
	}
}
void EditSectionScreen::updatePlaceStartPos() noexcept {
	if (IM::isMouseJustPressed(sf::Mouse::Left)) {
		sectionInfo_.startPos = getSnapedMouseCameraPos();
		exitToolState();
	}
}

void EditSectionScreen::updatePlaceStructure() noexcept {
	if (structureSwitcher->getCurrentPanel()->getName() == PortalInfo::JSON_ID) {
		updateDrawPortal();
	}
	else if (structureSwitcher->getCurrentPanel()->getName() == PlateformeInfo::JSON_ID) {
		updateDrawPlateform();
	}
}

void EditSectionScreen::updateDrawPortal() noexcept {
	if (portalFirstPoint) {
		if (!IM::isMousePressed(sf::Mouse::Left)) {
			auto A = *portalFirstPoint;
			auto B = getSnapedMouseCameraPos();

			if (A != B) {
				PortalInfo info;
				info.frontier = { A, B };

				sectionInfo_.portals.push_back(info);
			}

			portalFirstPoint.reset();
		}
		if (IM::isMouseJustPressed(sf::Mouse::Right)) {
			portalFirstPoint.reset();
		}
		return;
	}

	if (IM::isMouseJustPressed(sf::Mouse::Left)) {
		portalFirstPoint = getSnapedMouseCameraPos();
	}
}

void EditSectionScreen::updateDrawPlateform() noexcept {
	if (_newPlateforme.has_value()) {
		_newPlateforme->rectangle.size =
			getSnapedMouseCameraPos() - _newPlateforme->rectangle.pos;
	}

	if (IM::isMouseJustPressed(sf::Mouse::Left)) {
		auto p = PlateformeInfo();
		p.rectangle.pos = getSnapedMouseCameraPos();

		_newPlateforme = p;
	}
	else if (IM::isMouseJustReleased(sf::Mouse::Left)) {
		if (_newPlateforme->rectangle.area() == 0.0) {
			_newPlateforme.reset();
			return;
		}
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
	}
}
void EditSectionScreen::updateNothing() noexcept {
	if (!input_caught_by_ui.test(Widget::Input_Mask::Mouse_Began)) {
		if (IM::isMouseJustPressed(sf::Mouse::Right)) {
			selectFocus();
		}
		if (
			IM::isMouseJustPressed(sf::Mouse::Left) && !draggingScreen) {
			draggingScreen = true;
		}
	}
	if (!input_caught_by_ui.test(Widget::Input_Mask::Mouse_Going)) {
		if (draggingScreen && IM::isMousePressed(sf::Mouse::Left)) {
			es_instance->get(cameraView)->move(
				-1 * IM::getMouseDeltaInView(*es_instance->get(cameraView))
			);
		}
		else draggingScreen = false;
	}
}
void EditSectionScreen::updateCameraMovement(double dt) noexcept {
	if (IM::isKeyPressed(sf::Keyboard::LControl)) return;

	float speedFactor = IM::isKeyPressed(sf::Keyboard::LShift) ? 3.f : 1.f;

	if (IM::isKeyPressed(sf::Keyboard::Z)) {
		es_instance->get(cameraView)->move(
			{ 0.f, -(float)(dt * sectionInfo_.maxRectangle.h / 3.f * speedFactor) }
		);
	}
	if (IM::isKeyPressed(sf::Keyboard::Q)) {
		es_instance->get(cameraView)->move(
			{ -(float)(dt * sectionInfo_.maxRectangle.w / 3.f) * speedFactor , 0.f }
		);
	}
	if (IM::isKeyPressed(sf::Keyboard::S)) {
		es_instance->get(cameraView)->move(
			{ 0.f, +(float)(dt * sectionInfo_.maxRectangle.h / 3.f) * speedFactor }
		);
	}
	if (IM::isKeyPressed(sf::Keyboard::D)) {
		es_instance->get(cameraView)->move(
			{ +(float)(dt * sectionInfo_.maxRectangle.w / 3.f) * speedFactor , 0.f}
		);
	}
	if (float delta = IM::getLastScroll(); delta != 0.f) {
		es_instance->get(cameraView)->zoom(std::powf(1.2f, -delta));
	}
}

void EditSectionScreen::inputSwitchState() noexcept {
	if (IM::isLastSequenceJustFinished({
			sf::Keyboard::LControl, sf::Keyboard::Q, sf::Keyboard::P
	})){
		if (_toolState != place_structure) {
			exitToolState();
			enterToolState(place_structure);
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
			// i don't know if this is usefull to prevent race condition.
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
	
	target.setView(*es_instance->get(cameraView));

	viewSize_.pos =
		(Vector2f)es_instance->get(cameraView)->getCenter() - viewSize_.size / 2.f;

	for (auto source : sectionInfo_.sourcesBoomerang) renderDebug(target, source.source);
	for (auto source : sectionInfo_.sourcesDirection) renderDebug(target, source.source);
	for (auto first_boss : sectionInfo_.first_bosses) renderDebug(target, first_boss);
	for (auto source : sectionInfo_.sourcesVaccum) renderDebug(target, source.source);
	for (auto plateforme : sectionInfo_.plateformes) renderDebug(target, plateforme);
	for (auto distance : sectionInfo_.distanceGuys) renderDebug(target, distance);
	for (auto source : sectionInfo_.sources) renderDebug(target, source);
	for (auto melee : sectionInfo_.meleeGuys) renderDebug(target, melee);
	for (auto p : sectionInfo_.navigationPoints) renderDebug(target, p);
	for (auto p : sectionInfo_.navigationLinks) renderDebug(target, p);
	for (auto slime : sectionInfo_.slimes) renderDebug(target, slime);
	for (auto fly : sectionInfo_.flies) renderDebug(target, fly);
	for (auto p : sectionInfo_.portals) renderDebug(target, p);

	if (_newPlateforme) renderDebug(target, *_newPlateforme);
	if (_newSource) renderDebug(target, *_newSource);
	if (portalFirstPoint)
		Segment2f{ *portalFirstPoint, getSnapedMouseCameraPos() }
			.render(target, { 1, 1, 1, 1 });

	if (_toolState == place_ennemy)
		getSnapedMouseCameraPos().plot(target, 0.1f, { 1.0, 0.0, 0.0, 1.0 }, {}, 0.f);
	if (_toolState == place_navigation_point)
		getSnapedMouseCameraPos().plot(target, 0.15f, { 1.0, 0.0, 0.0, 1.0 }, {}, 0.f);

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

	Vector2f startPos = sectionInfo_.startPos;
	if (_toolState == place_start_pos) {
		startPos = getSnapedMouseCameraPos();
	}
	startPos.plot(target, 0.1f, { 1.0, 0.0, 0.0, 1.0 }, {}, 0.f);

	viewSize_.render(
		target, Vector4f{ 0.f, 0.f, 0.f, 0.f }, Vector4f{ 1.f, 0.f, 0.f, 1.f }
	);

	target.setView(_uiView);
	for (auto&[_, w] : _widgets) {
		_;
		w->propagateRender(target);
	}

	target.setView(old);
}

void EditSectionScreen::onEnter(std::any x) {
	Screen::onEnter(x);
	_json = AM::getJson("editScreen");

	constructUI();
	focused_screen = true;
}

std::any EditSectionScreen::onExit() {
	if (!focused_screen) return ReturnType{};
	filepath_ = _savePicker->getStdString();
	for (auto&[key, value] : _widgets) {
		delete value;
		value = nullptr;
	}
	focused_screen = false;
	return ReturnType{sectionInfo_, filepath_};
}

void EditSectionScreen::enterToolState(
	EditSectionScreen::EditorToolState toolState
) noexcept {
	_toolState = toolState;
	switch (toolState)
	{
	case place_structure:
		structureSwitcher->setVisible(true);
		changeColorLabel(PLACE_STRUCTURE, { 0.0, 1.0, 0.0, 1.0 });
		break;
	case place_ennemy:
		ennemySwitcher_->setVisible(true);
		changeColorLabel(PLACE_SLIME, { 0.0, 1.0, 0.0, 1.0 });
		break;
	case place_start_pos:
		changeColorLabel(PLACE_START_POS, { 0.0, 1.0, 0.0, 1.0 });
		break;
	case place_source:
		_newSource = SourceInfo::loadJson(AM::getJson("source"));
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
	case place_structure:
		structureSwitcher->setVisible(false);
		_newPlateforme.reset();
		portalFirstPoint.reset();
		changeColorLabel(PLACE_STRUCTURE, { 1.0, 1.0, 1.0, 1.0 });
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
	sf::RenderTarget& target, const PlateformeInfo& info
) noexcept {
	Vector4f color{ 0.8f, 0.8f, 0.0f, info.passable ? 0.5f : 1.0f };
	if (info.rectangle.in(IM::getMousePosInView(*es_instance->get(cameraView)))) {
		color = { 1.0f, 0.7f, 0.0f, info.passable ? 0.5f : 1.0f };
	}
	info.rectangle.render(target, color);
}
void EditSectionScreen::renderDebug(
	sf::RenderTarget& target, const SlimeInfo& info
) noexcept {
	Vector4d color{ 0.0, 0.8, 0.8, 1.0 };
	auto dist2 = (info.startPos - IM::getMousePosInView(*es_instance->get(cameraView)))
		.length2();
	if (dist2 < info.size.x * info.size.x / 4.f) {
		color = { 0.0, 0.7, 1.0, 1.0 };
	}
	info.startPos.plot(
		target, info.size.x / 2.f, color, { 0.0, 0.0, 0.0, 0.0 }, 0.f
	);
}
void EditSectionScreen::renderDebug(
	sf::RenderTarget& target, const DistanceGuyInfo& info
) noexcept {
	Vector4d color{ 0.1, 0.7, 0.7, 1.0 };
	auto dist2 = (info.startPos - IM::getMousePosInView(*es_instance->get(cameraView)))
		.length2();
	if (dist2 < info.size.x * info.size.x / 4.f) {
		color = { 0.0, 0.6, 0.9, 1.0 };
	}
	info.startPos.plot(
		target, info.size.x / 2.f, color, { 0.0, 0.0, 0.0, 0.0 }, 0.f
	);
}
void EditSectionScreen::renderDebug(
	sf::RenderTarget& target, const MeleeGuyInfo& info
) noexcept {
	Vector4d color{ 0.1, 0.7, 0.7, 1.0 };
	auto dist2 = (info.startPos - IM::getMousePosInView(*es_instance->get(cameraView)))
		.length2();
	if (dist2 < info.size.x * info.size.x / 4.f) {
		color = { 0.0, 0.6, 0.9, 1.0 };
	}
	info.startPos.plot(
		target, info.size.x / 2.f, color, { 0.0, 0.0, 0.0, 0.0 }, 0.f
	);
}
void EditSectionScreen::renderDebug(
	sf::RenderTarget& target, const FlyInfo& info
) noexcept {
	Vector4d color{ 0.1, 0.7, 0.7, 1.0 };
	auto dist2 = (info.startPos - IM::getMousePosInView(*es_instance->get(cameraView)))
		.length2();
	if (dist2 < info.radius * info.radius / 4.f) {
		color = { 0.0, 0.6, 0.9, 1.0 };
	}
	info.startPos.plot(
		target, info.radius / 2.f, color, { 0.0, 0.0, 0.0, 0.0 }, 0.f
	);
}
void EditSectionScreen::renderDebug(
	sf::RenderTarget& target, const SourceInfo& info
) noexcept {
sf::Sprite spr{ AM::getTexture(info.texture) };
	spr.setPosition(info.pos);
	spr.setOrigin(
		info.origin.x * spr.getTextureRect().width,
		info.origin.y * spr.getTextureRect().height
	);
	spr.setScale(
		info.size.x / spr.getTextureRect().width,
		info.size.y / spr.getTextureRect().height
	);
	
	auto mouse_pos = IM::getMousePosInView(*es_instance->get(cameraView));
	if (is_in(mouse_pos, info)) {
		spr.setColor(Vector4d{ 1, 1, 1, 1 });
	}
	else {
		spr.setColor(Vector4d{ 0.8, 0.8, 0.8, 1.0 });
	}

	target.draw(spr);
}
void EditSectionScreen::renderDebug(
	sf::RenderTarget& target, const PortalInfo& info
) noexcept {
	auto color = Vector4d{ 1, 1, 1, 1 };
	if (is_in_ellipse(
		info.frontier.A,
		info.frontier.B,
		info.frontier.length() * 0.01f,
		IM::getMousePosInView(*es_instance->get(cameraView))
	)) color = { 0.8, 0.2, 0.8, 0.8 };
	
	info.frontier.render(target, color);
}
void EditSectionScreen::renderDebug(
	sf::RenderTarget& target, const NavigationPointInfo& info
) noexcept {
	Vector4d color{ 0.8, 0.0, 0.8, 1.0 };
	auto dist2 = (info.pos - IM::getMousePosInView(*es_instance->get(cameraView)))
		.length2();
	if (dist2 < info.range * info.range) {
		color = { 0.7, 0.0, 1.0, 1.0 };
	}
	info.pos.plot(target, info.range, color, { 0.0, 0.0, 0.0, 0.0 }, 0.f);
}
void EditSectionScreen::renderDebug(
	sf::RenderTarget& target, const NavigationLinkInfo& info
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

	if (is_in_ellipse(
		a->pos, b->pos, (a->pos - b->pos).length() * 0.001f, IM::getMouseScreenPos()
	)) {
		color = { 0.7, 0.0, 1.0, 1.0 };
	}

	Vector2f::renderLine(target, a->pos, b->pos, color);
}
void EditSectionScreen::renderDebug(
	sf::RenderTarget& target, const FirstBossInfo& info
) noexcept {
	Vector4d color{ 1.0, 0.7, 0.7, 1.0 };
	auto dist = IM::getMousePosInView(*es_instance->get(cameraView)) - info.start_pos;
	if (info.hitbox.in(dist)) {
		color = { 0.8, 0.6, 0.9, 1.0 };
	}
	info.start_pos.plot(target, info.hitbox.w / 2.f, color, { 0.0, 0.0, 0.0, 0.0 }, 0.f);
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

void EditSectionScreen::selectFocus() noexcept {
	auto& plateformes = sectionInfo_.plateformes;
	auto& portals = sectionInfo_.portals;
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
	auto& first_bosses = sectionInfo_.first_bosses;
	
	currentlyFocused = std::nullopt;
	auto mouse_pos_in_camera_view = IM::getMousePosInView(*es_instance->get(cameraView));

	for (size_t i = plateformes.size(); i > 0; --i) {
		if (plateformes[i - 1].rectangle.in(mouse_pos_in_camera_view)) {
			currentlyFocused = {
				holded_t<decltype(plateformes)>::JSON_ID,
				(void*)&plateformes[i - 1]
			};
		}
	}

	for (size_t i = slimes.size(); i > 0; --i) {

		auto dist2 =
			(slimes[i - 1].startPos - mouse_pos_in_camera_view).length2();
		if (dist2 < slimes[i - 1].size.x * slimes[i - 1].size.x / 4.f) {
			currentlyFocused = {
				holded_t<decltype(slimes)>::JSON_ID,
				(void*)&slimes[i - 1]
			};
		}
	}
	for (size_t i = distance.size(); i > 0; --i) {

		auto dist2 = (distance[i - 1].startPos - mouse_pos_in_camera_view).length2();
		if (dist2 < distance[i - 1].size.x * distance[i - 1].size.x / 4.f) {
			currentlyFocused = {
				holded_t<decltype(distance)>::JSON_ID,
				(void*)&distance[i - 1]
			};
		}
	}
	for (size_t i = meleeGuy.size(); i > 0; --i) {
		auto dist2 = (meleeGuy[i - 1].startPos - mouse_pos_in_camera_view).length2();
		if (dist2 < meleeGuy[i - 1].size.x * meleeGuy[i - 1].size.x / 4.f) {
			currentlyFocused = {
				holded_t<decltype(meleeGuy)>::JSON_ID,
				(void*)&meleeGuy[i - 1]
			};
		}
	}
	for (size_t i = flies.size(); i > 0; --i) {
		auto dist2 = (flies[i - 1].startPos - mouse_pos_in_camera_view).length2();
		if (dist2 < flies[i - 1].radius * flies[i - 1].radius / 4.f) {
			currentlyFocused = {
				holded_t<decltype(flies)>::JSON_ID,
				(void*)&flies[i - 1]
			};
		}
	}

	for (size_t i = sources.size() - 1; i + 1 > 0; --i) {
		if (is_in(mouse_pos_in_camera_view, sources[i])) {
			currentlyFocused = { holded_t<decltype(sources)>::JSON_ID, (void*)&sources[i] };
		}
	}

	for (size_t i = sourcesBoomerang.size() - 1; i + 1 > 0; --i) {
		if (is_in(mouse_pos_in_camera_view, sourcesBoomerang[i].source)) {
			currentlyFocused = {
				holded_t<decltype(sourcesBoomerang)>::JSON_ID, (void*)&sourcesBoomerang[i]
			};
		}
	}
	for (size_t i = sourcesVaccum.size() - 1; i + 1 > 0; --i) {
		if (is_in(mouse_pos_in_camera_view, sourcesVaccum[i].source)) {
			currentlyFocused = {
				holded_t<decltype(sourcesVaccum)>::JSON_ID, (void*)&sourcesVaccum[i]
			};
		}
	}
	for (size_t i = sourcesDirection.size() - 1; i + 1 > 0; --i) {
		if (is_in(mouse_pos_in_camera_view, sourcesDirection[i].source)) {
			currentlyFocused = {
				holded_t<decltype(sourcesDirection)>::JSON_ID, (void*)&sourcesDirection[i]
			};
		}
	}
	for (size_t i = navigationPoints.size(); i > 0; --i) {
		auto dist2 = (navigationPoints[i - 1].pos - mouse_pos_in_camera_view).length2();

		if (dist2 < navigationPoints[i - 1].range * navigationPoints[i - 1].range) {
			currentlyFocused = {
				holded_t<decltype(navigationPoints)>::JSON_ID,
				(void*)&navigationPoints[i - 1]
			};
		}
	}
	for (size_t i = first_bosses.size(); i > 0; --i) {
		auto dist = mouse_pos_in_camera_view - first_bosses[i - 1].start_pos;
		if (first_bosses[i - 1].hitbox.in(dist)) {
			currentlyFocused = {
				holded_t<decltype(first_bosses)>::JSON_ID,
				(void*)&first_bosses[i - 1]
			};
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

		bool test = is_in_ellipse(
			a->pos, b->pos, (a->pos - b->pos).length() * 0.001f, IM::getMouseScreenPos()
		);

		if (test) {
			currentlyFocused = {
				holded_t<decltype(navigationLinks)>::JSON_ID,
				(void*)&navigationLinks[i - 1]
			};
		}
	}
	for (size_t i = portals.size(); i > 0; --i) {
		auto p = portals[i - 1];
		auto m = IM::getMousePosInView(*es_instance->get(cameraView));

		if (is_in_ellipse(p.frontier.A, p.frontier.B, p.frontier.length() * 0.01f, m)) {
			currentlyFocused = {
				holded_t<decltype(portals)>::JSON_ID,
				(void*)&portals[i - 1]
			};
		}
	}

	applyToFocused([&](auto focused) {
		using type = std::decay_t<decltype(*focused)>;

		jsonEditPanel->killDirectChild(root_properties_name);
		JsonTree* json_tree = jsonEditPanel->makeChild<JsonTree>({
			{"pos", Vector2f{0, jsonEditPanel->getSize().y * 0.1f + 5}},
			{"name", root_properties_name},
			{"structure", type::saveJson(*focused)}
		});
		if (jsonEditPanel->isCollapsed()) json_tree->setVisible(false);

		auto open = json_tree->getChilds();
		while (!open.empty()) {
			auto w = open.back().second;
			open.pop_back();

			open.insert(
				std::end(open), std::begin(w->getChilds()), std::end(w->getChilds())
			);

			if (auto ptr = dynamic_cast<PosPicker*>(w); ptr) {
				ptr->setViews(std::vector{(Eid<sf::View>)cameraView});
			}
		}
	});
	if (!currentlyFocused) {
		jsonEditPanel->killDirectChild(root_properties_name);
	}
}
void EditSectionScreen::deleteHovered() noexcept {
	auto& plateformes = sectionInfo_.plateformes;
	auto& portals = sectionInfo_.portals;
	auto& slimes = sectionInfo_.slimes;
	auto& distance = sectionInfo_.distanceGuys;
	auto& meleeGuy = sectionInfo_.meleeGuys;
	auto& flies = sectionInfo_.flies;
	auto& sources = sectionInfo_.sources;
	auto& sourcesBoomerang = sectionInfo_.sourcesBoomerang;
	auto& sourcesDirection = sectionInfo_.sourcesDirection;
	auto& first_bosses = sectionInfo_.first_bosses;
	auto& sourcesVaccum = sectionInfo_.sourcesVaccum;
	auto& navigationPoints = sectionInfo_.navigationPoints;
	auto& navigationLinks = sectionInfo_.navigationLinks;

	auto mouse_pos_in_camera_view = IM::getMousePosInView(*es_instance->get(cameraView));

	for (size_t i = plateformes.size(); i > 0; --i) {
		if (plateformes[i - 1].rectangle.in(mouse_pos_in_camera_view)) {
			plateformes.erase(std::begin(plateformes) + i - 1);
			return;
		}
	}

	for (size_t i = slimes.size(); i > 0; --i) {

		auto dist2 = (slimes[i - 1].startPos - mouse_pos_in_camera_view).length2();
		if (dist2 < slimes[i - 1].size.x * slimes[i - 1].size.x / 4.f) {
			slimes.erase(std::begin(slimes) + i - 1);
			return;
		}
	}
	for (size_t i = distance.size(); i > 0; --i) {

		auto dist2 = (distance[i - 1].startPos - mouse_pos_in_camera_view).length2();
		if (dist2 < distance[i - 1].size.x * distance[i - 1].size.x / 4.f) {
			distance.erase(std::begin(distance) + i - 1);
			return;
		}
	}
	for (size_t i = first_bosses.size(); i > 0; --i) {
		auto dist = mouse_pos_in_camera_view - first_bosses[i - 1].start_pos;
		if (first_bosses[i - 1].hitbox.in(dist)) {
			first_bosses.erase(std::begin(first_bosses) + i - 1);
			return;
		}
	}
	for (size_t i = meleeGuy.size(); i > 0; --i) {
		auto dist2 = (meleeGuy[i - 1].startPos - mouse_pos_in_camera_view).length2();
		if (dist2 < meleeGuy[i - 1].size.x * meleeGuy[i - 1].size.x / 4.f) {
			meleeGuy.erase(std::begin(meleeGuy) + i - 1);
			return;
		}
	}
	for (size_t i = flies.size(); i > 0; --i) {
		auto dist2 = (flies[i - 1].startPos - mouse_pos_in_camera_view).length2();
		if (dist2 < flies[i - 1].radius * flies[i - 1].radius / 4.f) {
			flies.erase(std::begin(flies) + i - 1);
			return;
		}
	}

	for (size_t i = sources.size() - 1; i + 1 > 0; --i) {
		if (is_in(mouse_pos_in_camera_view, sources[i])) {
			sources.erase(std::begin(sources) + i);
			return;
		}
	}

	for (size_t i = sourcesBoomerang.size() - 1; i + 1 > 0; --i) {
		if (is_in(mouse_pos_in_camera_view, sourcesBoomerang[i].source)) {
			sourcesBoomerang.erase(std::begin(sourcesBoomerang) + i);
			return;
		}
	}
	for (size_t i = sourcesVaccum.size() - 1; i + 1 > 0; --i) {
		if (is_in(mouse_pos_in_camera_view, sourcesVaccum[i].source)) {
			sourcesVaccum.erase(std::begin(sourcesVaccum) + i);
			return;
		}
	}
	for (size_t i = sourcesDirection.size() - 1; i + 1 > 0; --i) {
		if (is_in(mouse_pos_in_camera_view, sourcesDirection[i].source)) {
			sourcesDirection.erase(std::begin(sourcesDirection) + i);
			return;
		}
	}
	for (size_t i = navigationPoints.size(); i > 0; --i) {
		auto dist2 =(navigationPoints[i - 1].pos - mouse_pos_in_camera_view).length2();

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

		bool test = is_in_ellipse(
			a->pos, b->pos, (a->pos - b->pos).length() * 0.01f, IM::getMouseScreenPos()
		);

		if (test) {
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
	for (size_t i = portals.size(); i > 0; --i) {
		auto p = portals[i - 1];
		auto m = IM::getMousePosInView(*es_instance->get(cameraView));

		if (is_in_ellipse(p.frontier.A, p.frontier.B, p.frontier.length() * 0.01f, m)) {
			portals.erase(portals.begin() + i - 1);
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
	auto pos = IM::getMousePosInView(*es_instance->get(cameraView));

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

void EditSectionScreen::constructUI() noexcept {
	for (auto&[key, value] : _json.get<nlohmann::json::object_t>()) {
		auto json = value;
		if (auto it = json.find("type"); it != json.end()) {
			auto type = it->get<std::string>();

			// You can't pass type to a local lambda as argument :(
#define X(x) if (type == x::NAME) _widgets[key] = new x{ json };
			X(Label);
			X(Panel);
			X(Button);
			X(PosPicker);
			X(ValuePicker);
			X(SpriteSwitcher);
			X(dyn_structTree);
#undef X
		}
		else {
			_widgets[key] = new Widget(json);
		}
	}

	root_ui = _widgets.at("root");
	assert(root_ui);

	structureSwitcher = (SpriteSwitcher*)(root_ui->findChild("structureSwitcher"));
	ennemySwitcher_ = (SpriteSwitcher*)(root_ui->findChild("ennemySwitcher"));
	sourceSwitcher_ = (SpriteSwitcher*)(root_ui->findChild("sourceSwitcher"));
	jsonEditPanel = (Panel*)_widgets.at("propertiesEditor");
	metadata_edit_panel = (Panel*)_widgets.at("metadata_editor");
	_savePicker = (ValuePicker*)(_widgets.at("root")->findChild("savePicker"));
	_savePicker->setStdString(filepath_.generic_string());
	_snapGrid = (Label*)(_widgets.at("root")->findChild("snapGrid"));

	auto confirmButton = (Button*)jsonEditPanel->findChild("confirm");

	Widget::Callback onClick;
	onClick.ended = std::bind(&EditSectionScreen::onClickEndedConfirmJsonEditPanel, this);
	confirmButton->setOnClick(onClick);

	auto* d_structTree = 
		(dyn_structTree*)metadata_edit_panel->findChild(root_properties_name);
	d_structTree->set_dyn_struct(sectionInfo_.meta_data);

	if (has(sectionInfo_.meta_data, "filepath")) {
		filepath_ = (std::string)get("filepath", sectionInfo_.meta_data);
	}
}

bool EditSectionScreen::onClickEndedConfirmJsonEditPanel() noexcept {
	if (!jsonEditPanel->haveChild(root_properties_name)) return true;

	auto root = ((JsonTree*)jsonEditPanel->findChild(root_properties_name));
	auto json = root->getJson();

	applyToFocused([json](auto focused) {
		*focused = std::decay_t<decltype(*focused)>::loadJson(json);
	});

	return true;
}
