#include "Screens/LevelScreen.hpp"

#include <tuple>

#include "Global/Const.hpp"
#include "Managers/AssetsManager.hpp"
#include "Managers/TimerManager.hpp"
#include "Gameplay/Weapon.hpp"
#include "Gameplay/Player.hpp"
#include "Gameplay/Level.hpp"
#include "Gameplay/Game.hpp"
#include "Gameplay/Boss.hpp"

LevelScreen::LevelScreen(uint32 n) :
	_n(n),
	_bossHealthTileSprite(AssetsManager::getTexture("health_tile")){

	_gameViewPos = { C::WIDTH / 2.f, C::HEIGHT / 2.f };
	_gameViewSize = { static_cast<float>(C::WIDTH), static_cast<float>(C::HEIGHT) };
	_guiView = sf::View(_gameViewPos, _gameViewSize);
	_gameView = sf::View(_gameViewPos, _gameViewSize);

	constexpr float bossHealthTileSize = 60;
	AssetsManager::getTexture("health_tile").setSmooth(true);
	_bossHealthTileSprite.setOrigin(_bossHealthTileSprite.getTextureRect().width * 0.5f, _bossHealthTileSprite.getTextureRect().height * 0.5f);
	_bossHealthTileSprite.setScale(bossHealthTileSize / _bossHealthTileSprite.getTextureRect().width, bossHealthTileSize / _bossHealthTileSprite.getTextureRect().height);
	_bossHealthTileSprite.setPosition(35, 35);

	_level = std::make_shared<Level>(*Level::levels[_n]);
}

LevelScreen::~LevelScreen() {
}

void LevelScreen::onEnter() {
	_level->start(this); // :(
	
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
	
	for (int i = 0; i < player->_maxLife; i++) {
		_playerLife[i].setFillColor(i < player->_life ? sf::Color(180, 50, 50) : sf::Color(90, 20, 20));
		_playerLife[i].setPosition(10 + i * (_playerLife[i].getSize().x + 5), (C::HEIGHT - 10) - _playerLife[i].getSize().y);
	}
	if(_level) _level->update(dt);
}
void LevelScreen::render(sf::RenderTarget& target) {
	_gameView.setCenter(_gameViewPos + _gameViewOffset);
	target.setView(_gameView);
	if (_level) _level->render(target);

	target.setView(_guiView);

	renderGui(target);

	for(auto& i : _playerLife)
		target.draw(i);
}
void LevelScreen::renderGui(sf::RenderTarget& target) {
	constexpr uint32_t nTiles = 10;
	constexpr float padding = -3;
	constexpr std::tuple<float, float, float> colors[] = {
		{0.f, 0.f, 0.f},
		{0.31f, 0.f, 0.f},
		{0.73f, 0.31f, 0.f},
		{1.f, 0.73f, 0.31f}
	};
	constexpr uint32_t nColors = sizeof(colors) / sizeof(std::tuple<float, float, float>) - 1;

	const auto pos = _bossHealthTileSprite.getPosition();
	const float& bossTileWidth = _bossHealthTileSprite.getGlobalBounds().width;
	const float& bossLife = _level->getNormalizedBossLife() == 1.f ? 1.f - FLT_EPSILON : _level->getNormalizedBossLife(); // hack to bypass the problem below
	
	for (uint32_t i = 0; i < nTiles; ++i) { // This depends on the fact that if a = n*b then  a mod 1 / n == b, need to check if it's true for float
		const uint32_t& color = (uint32_t)(bossLife * nColors) +  // Not true for nColors == 3 :(
								(((float)i / (nTiles * nColors)) < fmodf(bossLife, 1.f / nColors) ? 1 : 0);
		const float& r = std::get<0>(colors[color]); // ughh i wish that intelissens could do: const auto& [r, g, b] = tuple;
		const float& g = std::get<1>(colors[color]);
		const float& b = std::get<2>(colors[color]); 

		_bossHealthTileSprite.setColor(sf::Color((uint8_t)(r * 255), (uint8_t)(g * 255), (uint8_t)(b * 255)));
		target.draw(_bossHealthTileSprite);
		
		_bossHealthTileSprite.move(bossTileWidth + padding, 0);
	}
	_bossHealthTileSprite.setPosition(pos);

	auto& weaponGuiSprite = game->_player->_weapon->_uiSprite;
	weaponGuiSprite.setPosition({ WIDTH - 50.f, HEIGHT - 50.f });
	target.draw(weaponGuiSprite);

	for (auto& panel : _panels) {
		panel.render(target);
	}
}


void LevelScreen::shakeScreen(float power) {
	constexpr uint32_t nShakes = 5;
	constexpr float iTimeShakes = 0.03f;

	//TimerManager::addLinearEase(1.f, "", &_gameViewOffset, Vector2::ZERO, { 50, 0 });
	TimerManager::addFunction(iTimeShakes, "shake", [&, p = power, n = nShakes](float)mutable->bool {
		Vector2 to = Vector2::createUnitVector(unitaryRng(RNG) * 3 * PIf) * 5 * p;
		Vector2 from = _gameViewOffset;
		TimerManager::addLinearEase(iTimeShakes, "shake", &_gameViewOffset, from, to);
		if (n-- <= 0) 
			TimerManager::addLinearEase(iTimeShakes, "shakeBack", &_gameViewOffset, _gameViewOffset, Vector2::ZERO);
		return n == 0;
	});
}