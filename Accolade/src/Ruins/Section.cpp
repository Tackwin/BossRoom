#include "Section.hpp"

#include "./../Game.hpp"

#include "./../Managers/AssetsManager.hpp"
#include "./../Managers/EventManager.hpp"

#include "./Structure/Plateforme.hpp"

void Section::enter() noexcept {
	_cameraView = sf::View({ WIDTH / 2.f, HEIGHT / 2.f }, { (float)WIDTH, (float)HEIGHT });

	_player = std::make_shared<Player>(C::game->getPlayerInfo());
	_player->setPos({ 100.f, HEIGHT / 2.f });
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

void Section::loadJson(std::string jsonName) noexcept {
	_levelJson = AM::getJson(jsonName);

	for (auto[key, platform] : _levelJson.at("platforms").get<nlohmann::json::object_t>())
	{
		Vector2f pos;
		pos.x = (float)platform.at("pos").get<std::vector<double>>()[0];
		pos.y = (float)platform.at("pos").get<std::vector<double>>()[1];
		Vector2f size;
		size.x = (float)platform.at("size").get<std::vector<double>>()[0];
		size.y = (float)platform.at("size").get<std::vector<double>>()[1];

		auto box = std::make_shared<Plateforme>(pos, size);
		auto temp = std::shared_ptr<Structure>{ box };
		addStructure(temp);
	}
}

void Section::update(double dt) noexcept {
	_player->update(dt);
	for (auto& spatial : _structures) {
		spatial->update(dt);
	}
}
void Section::render(sf::RenderTarget& target) const noexcept {
	auto oldView = target.getView();
	target.setView(_cameraView);

	_player->render(target);

	target.setView(oldView);
}

void Section::renderDebug(sf::RenderTarget& target) const noexcept {
	for (auto& spatial : _structures) {
		spatial->renderDebug(target);
	}
	_world.render(target);
}

void Section::addStructure(std::shared_ptr<Structure>& ptr) noexcept {
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

void Section::playerOnEnter(Object*) noexcept {
}