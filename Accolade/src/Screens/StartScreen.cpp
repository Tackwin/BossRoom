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
	_playerView({ WIDTH / 2.f, HEIGHT / 2.f }, { (float)WIDTH, (float)HEIGHT }),
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
	_weaponLabel.setString("Weapon");
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
	_json = AssetsManager::getJson(JSON_KEY)["startZone"];
	_player = std::make_shared<Player>();

	_shop.setPlayer(_player);

	initializeSprite();
	initializeWorld();
	subscribeToEvents();

	_generator.loadJson(AM::getJson(JSON_KEY)["particlesGenerator"]["particleBurstFeather"]);
	_generator.setPos({ 100, 100 });
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

void StartScreen::initializeWorld(){
	_world.addObject(_player);
	_player->collider->onEnter =
		std::bind(&StartScreen::playerOnEnter, this, std::placeholders::_1);
	_player->collider->onExit =
		std::bind(&StartScreen::playerOnExit, this, std::placeholders::_1);

	_floor = std::make_shared<Object>();
	_floor->idMask.set((size_t)Object::BIT_TAGS::FLOOR);
	_floor->pos = { 0, 600 };
	_floor->collider = std::make_unique<Box>();
	((Box*)_floor->collider.get())->size = { 3000, 120 };

	_world.addObject(_floor);

	auto& merchant = _zones["merchant"];
	merchant = std::make_shared<Zone>();
	merchant->setRadius(150.f);
	merchant->pos.x = _merchantShop.getPosition().x +
		_merchantShop.getGlobalBounds().width * 0.5f;
	merchant->pos.y = _merchantShop.getPosition().y +
		_merchantShop.getGlobalBounds().height * 0.5f;
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

	if (_dungeonDoor.getGlobalBounds().contains(_player->pos)) {
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
	
	float minX = 
		_startBackground.getGlobalBounds().left + _playerView.getSize().x / 2.f;
	float maxX = 
		_startBackground.getGlobalBounds().left + 
		_startBackground.getGlobalBounds().width - 
		_playerView.getSize().x / 2.f;

	float minY = 
		_startBackground.getGlobalBounds().top + _playerView.getSize().y / 2.f;
	float maxY = _startBackground.getGlobalBounds().top + 
		_startBackground.getGlobalBounds().height - 
		_playerView.getSize().y / 2.f;

	_playerView.setCenter({
		std::clamp(_player->pos.x, minX, maxX),
		std::clamp(_player->pos.y, minY, maxY)
	});
	target.setView(_playerView);

	target.draw(_startBackground);
	target.draw(_dungeon);
	target.draw(_merchantShop);
	target.draw(_dungeonDoor);

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
		auto texture = Wearable::GetWearableinfo(_player->getWeapon()).uiTexture;
		_weaponIcon.setSprite(sf::Sprite{ AM::getTexture(texture) });
		_weaponLabel.setPosition(_weaponIcon.getSize() * (-1.f));
	}

	_guiRoot.propagateRender(target);

	target.setView(oldView);
}

void StartScreen::initializeSprite() {
	{
		const auto& json = _json["startBackground"];
		_startBackground = sf::Sprite(AssetsManager::getTexture(json["sprite"]));
		_startBackground.setScale(
			json["size"][0].get<float>() / _startBackground.getTextureRect().width,
			json["size"][1].get<float>() / _startBackground.getTextureRect().height
		);
		_startBackground.setPosition(
			json["pos"][0].get<float>(),
			json["pos"][1].get<float>()
		);
	} 
	{
		const auto& json = _json["dungeon"];
		_dungeon = sf::Sprite(AssetsManager::getTexture(json["sprite"]));
		_dungeon.setScale(
			json["size"][0].get<float>() / _dungeon.getTextureRect().width,
			json["size"][1].get<float>() / _dungeon.getTextureRect().height
		);
		_dungeon.setPosition(
			json["pos"][0].get<float>(),
			json["pos"][1].get<float>()
		);
	} 
	{
		const auto& json = _json["dungeonDoor"];
		_dungeonDoor = sf::Sprite(AssetsManager::getTexture(json["sprite"]));
		_dungeonDoor.setScale(
			json["size"][0].get<float>() / _dungeonDoor.getTextureRect().width,
			json["size"][1].get<float>() / _dungeonDoor.getTextureRect().height
		);
		_dungeonDoor.setPosition(
			json["pos"][0].get<float>(),
			json["pos"][1].get<float>()
		);
	} 
	{
		const auto& json = _json["merchantShop"];
		_merchantShop = sf::Sprite(AssetsManager::getTexture(json["sprite"]));
		_merchantShop.setScale(
			json["size"][0].get<float>() / _merchantShop.getTextureRect().width,
			json["size"][1].get<float>() / _merchantShop.getTextureRect().height
		);
		_merchantShop.setPosition(
			json["pos"][0].get<float>(),
			json["pos"][1].get<float>()
		);
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