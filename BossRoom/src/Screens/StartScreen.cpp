#include "Screens/StartScreen.hpp"

#include <algorithm>

#include "Managers/AssetsManager.hpp"
#include "Managers/InputsManager.hpp"
#include "Gameplay/Player.hpp"
#include "Gameplay/Game.hpp"
#include "Global/Const.hpp"

StartScreen::StartScreen():
	_playerView({ WIDTH / 2.f, HEIGHT / 2.f }, { (float)WIDTH, (float)HEIGHT }),
	_guiView({ WIDTH / 2.f, HEIGHT / 2.f }, { (float)WIDTH, (float)HEIGHT }) {

}


void StartScreen::onEnter() {
	_json = AssetsManager::getJson(JSON_KEY)["startZone"];

	initializeSprite();
	_player = game->_player;
	_player->initializeJson();
	_player->_pos = {100, 100};
}
void StartScreen::onExit() {
}
void StartScreen::update(float dt_) {
	_player->update(dt_);

	if (_dungeonDoor.getGlobalBounds().contains(_player->_pos)) {
		game->enterDungeon();
	}
}
void StartScreen::render(sf::RenderTarget& target_) {
	const auto oldView = target_.getView();
	
	float minX = _startBackground.getGlobalBounds().left + _playerView.getSize().x / 2.f;
	float maxX = _startBackground.getGlobalBounds().left + _startBackground.getGlobalBounds().width - _playerView.getSize().x / 2.f;

	float minY = _startBackground.getGlobalBounds().top + _playerView.getSize().y / 2.f;
	float maxY = _startBackground.getGlobalBounds().top + _startBackground.getGlobalBounds().height - _playerView.getSize().y / 2.f;

	_playerView.setCenter({
		std::clamp(_player->_pos.x, minX, maxX),
		std::clamp(_player->_pos.y, minY, maxY)
	});
	target_.setView(_playerView);

	target_.draw(_startBackground);
	target_.draw(_dungeon);
	target_.draw(_merchantShop);
	target_.draw(_dungeonDoor);
	game->_player->render(target_);

	target_.setView(oldView);
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
