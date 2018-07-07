#include "Section.hpp"

#include "./../Game.hpp"

#include "./../Managers/AssetsManager.hpp"
#include "./../Managers/EventManager.hpp"
#include "./Structure/Plateforme.hpp"


Section::Section(SectionInfo info) noexcept : _info(info) {
	_cameraView.setCenter(_info.maxRectangle.fitUpRatio(RATIO).center());
	_cameraView.setSize(_info.maxRectangle.fitUpRatio(RATIO).size);

	for (auto& plateforme : _info.plateformes) {
		auto ptr = std::make_shared<Plateforme>(plateforme);
		addStructure(std::shared_ptr<Structure>(ptr));
	}
}

void Section::enter() noexcept {
	_player = std::make_shared<Player>(C::game->getPlayerInfo());
	_player->setPos(_info.startPos);
	_player->collider->onEnter =
		std::bind(&Section::playerOnEnter, this, std::placeholders::_1);

	for (auto& structure : _structures) {
		_world.addObject(structure);
	}
	_world.addObject(_player);

	subscribeToEvents();
}

void Section::exit() noexcept {
	unsubscribeToEvents();

	_playerZones.clear();
	_projectiles.clear();
	_world.purge();
}

void Section::update(double dt) noexcept {
	_player->update(dt);
	for (auto& spatial : _structures) {
		spatial->update(dt);
	}

	_world.updateInc(dt, 1);
}
void Section::render(sf::RenderTarget& target) const noexcept {
	auto oldView = target.getView();
	target.setView(_cameraView);

	_player->render(target);

	target.setView(oldView);
}

void Section::renderDebug(sf::RenderTarget& target) const noexcept {
	auto oldView = target.getView();
	target.setView(_cameraView);

	for (auto& spatial : _structures) {
		spatial->renderDebug(target);
	}
	_world.render(target);

	target.setView(oldView);
}

void Section::addStructure(const std::shared_ptr<Structure>& ptr) noexcept {
	_structures.push_back(ptr);
}

void Section::subscribeToEvents() noexcept {
	_keyPressedEvent = EventManager::subscribe("keyPressed",
		[&](EventManager::EventCallbackParameter args) -> void {
		auto key = std::any_cast<sf::Keyboard::Key>(*args.begin());

		_player->keyPressed(key);
	}
	);
	_keyReleasedEvent = EventManager::subscribe("keyReleased",
		[&](EventManager::EventCallbackParameter args) -> void {
		auto key = std::any_cast<sf::Keyboard::Key>(*args.begin());
		_player->keyReleased(key);
	}
	);
}

void Section::unsubscribeToEvents() noexcept {
	EventManager::unSubscribe("keyPressed", _keyPressedEvent);
	EventManager::unSubscribe("keyReleased", _keyReleasedEvent);
}

void Section::playerOnEnter(Object* object) noexcept {
	if (object->idMask[Object::STRUCTURE]) {
		_player->floored();
	}
}
void Section::playerOnExit(Object*) noexcept {

}

SectionInfo SectionInfo::load(const nlohmann::json& json) noexcept {
	SectionInfo info;

	if (json.count("maxRect") != 0) {
		info.maxRectangle.x = json.at("maxRect").get<std::vector<float>>()[0];
		info.maxRectangle.y = json.at("maxRect").get<std::vector<float>>()[1];
		info.maxRectangle.w = json.at("maxRect").get<std::vector<float>>()[2];
		info.maxRectangle.h = json.at("maxRect").get<std::vector<float>>()[3];
	}

	if (json.count("plateformes") != 0) {
		info.plateformes.reserve(json.at("plateformes").size());
		for (auto[key, plateforme] : json.at("plateformes").get<nlohmann::json::object_t>())
		{
			info.plateformes.push_back(PlateformeInfo::loadFromJson(plateforme));
		}
	}

	if (json.count("startPos") != 0) {
		info.startPos.x = json.at("startPos").get<std::vector<float>>()[0];
		info.startPos.y = json.at("startPos").get<std::vector<float>>()[1];
	}

	return info;
}