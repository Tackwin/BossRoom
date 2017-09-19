#include "Screens/StartScreen.hpp"

#include <algorithm>

#include <Managers/AssetsManager.hpp>
#include <Managers/InputsManager.hpp>
#include <Gameplay/Projectile.hpp>
#include <Graphics/GUI/Button.hpp>
#include <Gameplay/Weapon.hpp>
#include <Gameplay/Player.hpp>
#include <Gameplay/Game.hpp>
#include <Global/Const.hpp>

StartScreen::StartScreen():
	_playerView({ WIDTH / 2.f, HEIGHT / 2.f }, { (float)WIDTH, (float)HEIGHT }),
	_guiView({ WIDTH / 2.f, HEIGHT / 2.f }, { (float)WIDTH, (float)HEIGHT })
	{

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
}


void StartScreen::onEnter() {
	_json = AssetsManager::getJson(JSON_KEY)["startZone"];

	initializeSprite();
	_player = game->_player;
	_player->initializeJson();
	_player->pos = {100, 100};
	_player->idMask |= Object::PLAYER;
	_player->collisionMask |= Object::FLOOR;

	_world.setPlayer(_player, 0);
	_worldExp.addObject(_player);

	_floor = std::make_shared<Object>();
	_floor->idMask |= Object::FLOOR;
	_floor->pos = { 0, 600 };
	Box* box = new Box();
	box->size = { 3000, 120 };
	_floor->collider = box;
	_worldExp.addObject(_floor);

	_world.addFloor({ 3000, 600 });

}
void StartScreen::onExit() {
	_player->_weapon->unEquip();
	_world.delPlayer(0);
}
void StartScreen::update(float dt) {
	
	//_world.update(dt);
	_worldExp.updateInc(dt, 10);
	_player->update(dt);


	if (_dungeonDoor.getGlobalBounds().contains(_player->pos)) {
		game->enterDungeon();
	}
}
void StartScreen::render(sf::RenderTarget& target) {
	const auto oldView = target.getView();
	
	float minX = _startBackground.getGlobalBounds().left + _playerView.getSize().x / 2.f;
	float maxX = _startBackground.getGlobalBounds().left + _startBackground.getGlobalBounds().width - _playerView.getSize().x / 2.f;

	float minY = _startBackground.getGlobalBounds().top + _playerView.getSize().y / 2.f;
	float maxY = _startBackground.getGlobalBounds().top + _startBackground.getGlobalBounds().height - _playerView.getSize().y / 2.f;

	_playerView.setCenter({
		std::clamp(_player->pos.x, minX, maxX),
		std::clamp(_player->pos.y, minY, maxY)
	});
	target.setView(_playerView);

	target.draw(_startBackground);
	target.draw(_dungeon);
	target.draw(_merchantShop);
	target.draw(_dungeonDoor);

	_worldExp.render(target);
	_player->render(target);

	renderGui(target);

	target.setView(oldView);
}
void StartScreen::renderGui(sf::RenderTarget& target) {
	const auto oldView = target.getView();
	target.setView(_guiView);
	
	_weaponIcon.setVisible((bool)_player->_weapon);
	if (_player->_weapon) {
		_weaponIcon.setSprite(_player->_weapon->_uiSprite);
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
	} {
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
	} {
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
	} {
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
