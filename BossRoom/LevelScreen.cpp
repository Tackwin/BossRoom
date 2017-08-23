#include "LevelScreen.hpp"

#include "AssetsManager.hpp"
#include "Systems.hpp"
#include "Weapon.hpp"
#include "Player.hpp"
#include "Const.hpp"
#include "Level.hpp"
#include "Game.hpp"
#include "Boss.hpp"

LevelScreen::LevelScreen(uint32 n) :
	_n(n){

	_gameViewPos = { C::WIDTH / 2.f, C::HEIGHT / 2.f };
	_gameViewSize = { static_cast<float>(C::WIDTH), static_cast<float>(C::HEIGHT) };
	_guiView = sf::View(_gameViewPos, _gameViewSize);
	_gameView = sf::View(_gameViewPos, _gameViewSize);


	_bossLifeShape.setPosition(20, 20);
	_bossLifeShape.setFillColor(sf::Color(140, 50, 50));
	_bossLifeShape.setOutlineColor(sf::Color(10, 30, 10));
	_bossLifeShape.setOutlineThickness(5);

	_level = std::make_shared<Level>(*Level::levels[_n]);
}

LevelScreen::~LevelScreen() {
}

void LevelScreen::onEnter() {
	_level->start();
	
	auto player = _level->_player;
	for (int i = 0; i < player->_maxLife; i++) {
		_playerLife.push_back(sf::RectangleShape());
		_playerLife[i].setOutlineColor(sf::Color(10, 10, 30));
		_playerLife[i].setSize({ 40, 40 });
		_playerLife[i].setOutlineThickness(2);
		_playerLife[i].setFillColor(i < player->_life ? sf::Color(180, 50, 50) : sf::Color(90, 20, 20));
		_playerLife[i].setPosition(10 + i * (_playerLife[i].getSize().x + 5), (C::HEIGHT - 10) - _playerLife[i].getSize().y);
	}
}
void LevelScreen::onExit() {
	if (_level) _level->stop();
	_playerLife.clear();
}

void LevelScreen::update(float dt) {
	if (_gameViewPos != _gameView.getCenter())
		_gameView.setCenter(_gameViewPos);
	if (_gameViewSize != _gameView.getSize())
		_gameView.setSize(_gameViewSize);
	
	auto boss = _level->_boss;
	auto player = _level->_player;
	_bossLifeShape.setSize({boss->_life * (C::WIDTH - 40.f) / boss->_maxLife, 50.f });
	
	for (int i = 0; i < player->_maxLife; i++) {
		_playerLife[i].setFillColor(i < player->_life ? sf::Color(180, 50, 50) : sf::Color(90, 20, 20));
		_playerLife[i].setPosition(10 + i * (_playerLife[i].getSize().x + 5), (C::HEIGHT - 10) - _playerLife[i].getSize().y);
	}
	if(_level) _level->update(dt);
}
void LevelScreen::render(sf::RenderTarget& target) {
	target.setView(_gameView);
	if (_level) _level->render(target);

	target.setView(_guiView);

	target.draw(game->_player->_weapon->_uiSprite);
	target.draw(_bossLifeShape);

	for(auto &i : _playerLife)
		target.draw(i);

}