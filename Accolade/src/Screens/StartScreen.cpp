#include "StartScreen.hpp"

#include <algorithm>

#include "./../Common.hpp"

#include "./../Game.hpp"

#include "./../Managers/AssetsManager.hpp"
#include "./../Managers/InputsManager.hpp"
#include "./../Managers/EventManager.hpp"

#include "./../Graphics/GUI/Button.hpp"

#include "./../Gameplay/Projectile.hpp"
#include "./../Gameplay/Wearable/Wearable.hpp"
#include "./../Gameplay/Player/Player.hpp"
#include "./../Gameplay/Zone.hpp"

StartScreen::StartScreen() :
	_guiView({ WIDTH / 2.f, HEIGHT / 2.f }, { (float)WIDTH, (float)HEIGHT })
{
	initializeGui();
}

void StartScreen::initializeGui() {
	_guiRoot.setPosition({ 0.f, 0.f });

	_weaponIcon.setOrigin({ 1.f, 1.f });
	_weaponIcon.setPosition({ (float)WIDTH, (float)HEIGHT});
	
	_weaponLabel.setFont("consola");
	_weaponLabel.setCharSize(15u);
	_weaponLabel.setStdString("Weapon");
	_weaponLabel.setOrigin({ 0.f, 1.f });
	_weaponLabel.setPosition(_weaponIcon.getSize() * (-1.f));

	_weaponIcon.addChild(&_weaponLabel);

	_guiRoot.addChild(&_weaponIcon);
	_guiRoot.addChild(&_shop);

	for (u32 i = 0u; i < 10; ++i) {
		for (auto& k : Wearable::EVERY_WEARABLE) {
			_shop.addWeapon(k);
		}
	}
}

void StartScreen::onEnter() {
	_json = AssetsManager::getJson("startZone");
	auto startingPos = _json.at("startPos").get<std::vector<double>>();

	Rectangle2f viewRectangle;
	viewRectangle.x = (float)_json["maxRectangle"].get<std::vector<double>>()[0];
	viewRectangle.y = (float)_json["maxRectangle"].get<std::vector<double>>()[1];
	viewRectangle.w = (float)_json["maxRectangle"].get<std::vector<double>>()[2];
	viewRectangle.h = (float)_json["maxRectangle"].get<std::vector<double>>()[3];
	
	_playerView.setSize(viewRectangle.fitDownRatio(RATIO).size);
	_playerView.setCenter(viewRectangle.center());

	_player = std::make_shared<Player>();
	_player->setPos({ (float)startingPos[0], (float)startingPos[1] });

	_shop.setPlayer(_player);

	initializeSprite();
	initializeWorld();
	subscribeToEvents();

	_generator.loadJson(AM::getJson(JSON_KEY)["particlesGenerator"]["particleBurstFeather"]);
	_generator.setPos({ 1, 1 });
}
void StartScreen::onExit() {
	unsubscribeToEvents();
	if (_enteredShop)
		unActivateShop();

	_player->collider->onEnter = [](Object*) {};
	_player->collider->onExit = [](Object*) {};
	_player->unmount();
	_projectiles.clear();
	_world.purge();
}

void StartScreen::initializeWorld() {
	_world.addObject(_player);
	_player->collider->onEnter =
		std::bind(&StartScreen::playerOnEnter, this, std::placeholders::_1);
	_player->collider->onExit =
		std::bind(&StartScreen::playerOnExit, this, std::placeholders::_1);

	auto json = _json.at("plateformes");

	for (auto&[key, value] : json.get<nlohmann::json::object_t>()) {
		PlateformeInfo info = PlateformeInfo::loadJson(value);

		_plateformes[key] = std::make_shared<Plateforme>(info);
		_world.addObject(_plateformes[key]);
	}

	auto& merchant = _zones["merchant"];

	json = AM::getJson("startZone").at("merchant");
	auto pos = json.at("pos").get<std::vector<double>>();
	auto size = json.at("size").get<std::vector<double>>();

	merchant = std::make_shared<Zone>();
	merchant->setRadius(json.at("radius"));
	merchant->pos.x = (float)(pos[0] + size[0] * 0.5);
	merchant->pos.y = (float)(pos[1] + size[1] * 0.5);
	merchant->collisionMask.set((size_t)Object::BIT_TAGS::PLAYER);
	merchant->collider->sensor = true;
	merchant->collider->onExit = [&](Object*) mutable { leaveShop(); };
	merchant->collider->onEnter = [&](Object*) mutable { enterShop(); };

	_world.addObject(_zones["merchant"]);
}



void StartScreen::update(double dt) {
	_generator.update(dt);
	removeNeeded();
	_guiRoot.propagateInput();
	if (_enteredShop && !_shop.isIn()) {
		unActivateShop();
	}

	pullPlayerObjects();

	auto mouse = InputsManager::getMousePosInView(_playerView);
	_player->setFacingDir((float)(mouse - _player->getPos()).angleX());
	_player->update(dt);
	for (auto& p : _projectiles) {
		p->update(dt);
	}

	_world.updateInc(dt, 1u);

	if (_enteredShop && InputsManager::isKeyJustPressed(sf::Keyboard::E)) {
		unActivateShop();
	}
	else if (_isInShop && InputsManager::isKeyJustPressed(sf::Keyboard::E)) {
		activateShop();
	}

	if (_sprites["dungeonDoor"].getGlobalBounds().contains(_player->pos)) {
		game->enterDungeon();
	}
}

void StartScreen::pullPlayerObjects() {
	for (auto& p : _player->getProtectilesToShoot()) {
		p->collider->onEnter = std::bind(
			&StartScreen::projectileOnEnter, this, std::weak_ptr(p), std::placeholders::_1
		);
		p->collider->onExit = std::bind(
			&StartScreen::projectileOnExit, this, std::weak_ptr(p), std::placeholders::_1
		);
		_projectiles.push_back(p);
		_world.addObject(p);
	}
	_player->clearProtectilesToShoot();

	for (auto& p : _player->getZonesToApply()) {
		_playerZones.push_back(p);
		_world.addObject(p);
	}
	_player->clearZonesToApply();
}

void StartScreen::render(sf::RenderTarget& target) {
	const auto oldView = target.getView();
	

	Rectangle2f maxRectangle;
	maxRectangle.x = (float)_json["maxRectangle"].get<std::vector<double>>()[0];
	maxRectangle.y = (float)_json["maxRectangle"].get<std::vector<double>>()[1];
	maxRectangle.w = (float)_json["maxRectangle"].get<std::vector<double>>()[2];
	maxRectangle.h = (float)_json["maxRectangle"].get<std::vector<double>>()[3];

	_playerView.setCenter(_player->getPos());

	Rectangle2f viewRect{
		_playerView.getCenter() - _playerView.getSize() / 2.f,
		_playerView.getSize()
	};

	Rectangle2f restirctedView = viewRect.restrictIn(maxRectangle);

	_playerView.setCenter(restirctedView.center());
	_playerView.setSize(restirctedView.size);

	target.setView(_playerView);

	target.draw(_sprites["startBackground"]);
	target.draw(_sprites["dungeon"]);
	target.draw(_sprites["merchantShop"]);
	target.draw(_sprites["dungeonDoor"]);

	for (auto& p : _projectiles) {
		p->render(target);
	}
	for (auto& [_, p] : _zones) {_;
		p->render(target);
	}
	for (auto& p : _playerZones) {
		p->render(target);
	}
	_player->render(target);

	renderGui(target);

	_world.render(target);
	_generator.render(target);

	target.setView(oldView);
}
void StartScreen::renderGui(sf::RenderTarget& target) {
	const auto oldView = target.getView();
	target.setView(_guiView);
	

	_weaponIcon.setVisible(_player->isEquiped());
	if (_player->isEquiped()) {
		auto texture = Wearable::GetWearableinfo(_player->getWeapon().value()).uiTexture;
		_weaponIcon.setSprite(sf::Sprite{ AM::getTexture(texture) });
		_weaponLabel.setPosition(_weaponIcon.getSize() * (-1.f));
	}

	_guiRoot.propagateRender(target);

	target.setView(oldView);
}

void StartScreen::initializeSprite() {
	for (auto&[key, value] : _json.at("sprites").get<nlohmann::json::object_t>()) {
		auto& sprite = _sprites[key];

		auto size = value.at("size").get<std::vector<double>>();
		auto pos = value.at("pos").get<std::vector<double>>();

		sprite.setTexture(AM::getTexture(value.at("sprite").get<std::string>()));
		sprite.setScale(
			(float)(size[0] / sprite.getTextureRect().width),
			(float)(size[1] / sprite.getTextureRect().height)
		);
		sprite.setPosition((float)pos[0], (float)pos[1]);
	}
}

void StartScreen::enterShop() {
	_isInShop = true;
}
void StartScreen::leaveShop() {
	_isInShop = false;
}

void StartScreen::removeNeeded() {
	for (size_t i = _projectiles.size(); i > 0; --i) {
		if (_projectiles[i - 1]->toRemove()) {
			_world.delObject(_projectiles[i - 1]->uuid);
			_projectiles.erase(_projectiles.begin() + i - 1);
		}
	}

	for (size_t i = _playerZones.size(); i > 0; --i) {
		if (_playerZones[i - 1]->toRemove()) {
			_world.delObject(_playerZones[i - 1]->uuid);
			_playerZones.erase(_playerZones.begin() + i - 1);
		}
	}
}

void StartScreen::activateShop() {
	_enteredShop = true;
	_player->setFocus(false);
	_shop.enter();
}

void StartScreen::unActivateShop() {
	_enteredShop = false;
	_player->setFocus(true);
	_shop.leave();
}

void StartScreen::playerOnEnter(Object* object) {
	if (object->idMask.test(Object::FLOOR)) {
		_player->floored();
	}
}

void StartScreen::playerOnExit(Object*) {}

void StartScreen::projectileOnEnter(std::weak_ptr<Projectile> projectile, Object*){
	if (projectile.expired()) return;
	projectile.lock()->remove();
}
void StartScreen::projectileOnExit(std::weak_ptr<Projectile> projectile, Object*){
	if (projectile.expired()) return;
	projectile.lock()->remove();
}

void StartScreen::subscribeToEvents() noexcept {
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

void StartScreen::unsubscribeToEvents() noexcept {
	EventManager::unSubscribe("keyPressed", _keyPressedEvent);
	EventManager::unSubscribe("keyReleased", _keyReleasedEvent);
}