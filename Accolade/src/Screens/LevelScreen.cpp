#include "Screens/LevelScreen.hpp"

#include <tuple>

#include "Game.hpp"
#include "Common.hpp"

#include "Managers/AssetsManager.hpp"
#include "Managers/TimerManager.hpp"

#include "Graphics/GUI/Panel.hpp"

#include "Gameplay/Projectile.hpp"
#include "Gameplay/Weapon.hpp"
#include "Gameplay/Player.hpp"
#include "Gameplay/Level.hpp"

LevelScreen::LevelScreen(u32 n) :
	_n(n),
	_bossHealthTileSprite(AssetsManager::getTexture("health_tile")),
	_gameViewPos({ WIDTH * 0.5f, HEIGHT * 0.5f}),
	_gameViewSize({ (float)WIDTH, (float)HEIGHT }),
	_guiView(_gameViewPos, _gameViewSize),
	_gameView(_gameViewPos, _gameViewSize),
	_level(MM::make_unique<Level>(*Level::levels[_n]))
{
	constexpr float bossHealthTileSize = 60;
	AssetsManager::getTexture("health_tile").setSmooth(true);
	_bossHealthTileSprite.setOrigin(
		_bossHealthTileSprite.getTextureRect().width * 0.5f, 
		_bossHealthTileSprite.getTextureRect().height * 0.5f
	);
	_bossHealthTileSprite.setScale(
		bossHealthTileSize / _bossHealthTileSprite.getTextureRect().width,
		bossHealthTileSize / _bossHealthTileSprite.getTextureRect().height
	);
	_bossHealthTileSprite.setPosition(35, 35);
}

LevelScreen::~LevelScreen() {
}

void LevelScreen::onEnter() {
	_level->start(this);
	
	auto player = game->_player;
	player->initializeJson();
	for (int i = 0; i < player->getPlayerInfo().maxLife; i++) {
		_playerLife.push_back(sf::RectangleShape());
		_playerLife[i].setOutlineColor(sf::Color(10, 10, 30));
		_playerLife[i].setSize({ 40, 40 });
		_playerLife[i].setOutlineThickness(2);
		_playerLife[i].setFillColor(
			i < player->getLife()
				? sf::Color(180, 50, 50)
				: sf::Color(90, 20, 20)
		);
		_playerLife[i].setPosition(
			10 + i * (_playerLife[i].getSize().x + 5),
			(C::HEIGHT - 10) - _playerLife[i].getSize().y
		);
	}
}

void LevelScreen::onExit() {
	if (_level) 
		_level->stop();
	_level.reset();

	_playerLife.clear();
}

void LevelScreen::update(double dt) {
	if (_gameViewPos != _gameView.getCenter())
		_gameView.setCenter(_gameViewPos);
	if (_gameViewSize != _gameView.getSize())
		_gameView.setSize(_gameViewSize);

	auto player = game->_player;

	for (int i = 0; i < player->getPlayerInfo().maxLife; i++) {
		_playerLife[i].setFillColor(
			i < player->getLife()
				? sf::Color(180, 50, 50) 
				: sf::Color(90, 20, 20)
		);
		_playerLife[i].setPosition(
			10 + i * (_playerLife[i].getSize().x + 5), 
			(C::HEIGHT - 10) - _playerLife[i].getSize().y
		);
	}

	if (_level) {
		_level->update(dt);
		if (_level->lost()) {
			game->enterHeritage();
		}
	}
}

void LevelScreen::render(sf::RenderTarget& target) {
	const auto& initialView = target.getView();

	_gameView.setCenter(_gameViewPos + _gameViewOffset);
	target.setView(_gameView);
	if (_level) _level->render(target);

	target.setView(_guiView);

	renderGui(target);

	for (auto& i : _playerLife)
		target.draw(i);

	target.setView(initialView);
}

void LevelScreen::renderGui(sf::RenderTarget& target) {
	constexpr u32 nTiles = 10;
	constexpr float padding = -3;
	constexpr std::tuple<float, float, float> colors[] = {
		{ 0.f, 0.f, 0.f },
		{ 0.31f, 0.f, 0.f },
		{ 0.73f, 0.31f, 0.f },
		{ 1.f, 0.73f, 0.31f }
	};
	constexpr u32 nColors = 
		sizeof(colors) / sizeof(std::tuple<float, float, float>) - 1;

	const auto pos = _bossHealthTileSprite.getPosition();
	const float& bossTileWidth = _bossHealthTileSprite.getGlobalBounds().width;
	const float& bossLife = 
		_level->getNormalizedBossLife() == 1.f 
			? 1.f - FLT_EPSILON 
			: _level->getNormalizedBossLife(); // hack to bypass the problem below

	for (u32 i = 0; i < nTiles; ++i) { // This depends on the fact that if a = n*b then  a mod 1 / n == b, need to check if it's true for float
		const u32& color = (u32)(bossLife * nColors) +  // Not true for nColors == 3 :(
		(
			((float)i / (nTiles * nColors)) < fmodf(bossLife, 1.f / nColors) 
				? 1 
				: 0
		);
		const float& r = std::get<0>(colors[color]); // ughh i wish that intelissens could do: const auto& [r, g, b] = tuple;
		const float& g = std::get<1>(colors[color]);
		const float& b = std::get<2>(colors[color]);

		_bossHealthTileSprite.setColor(sf::Color(
			(u08)(r * 255), 
			(u08)(g * 255), 
			(u08)(b * 255)
		));
		target.draw(_bossHealthTileSprite);

		_bossHealthTileSprite.move(bossTileWidth + padding, 0);
	}
	_bossHealthTileSprite.setPosition(pos);
	

	sf::Sprite weaponGuiSprite = game->_player->_weapon->getUiSprite();
	weaponGuiSprite.setPosition({ WIDTH - 50.f, HEIGHT - 50.f });
	target.draw(weaponGuiSprite);
}

void LevelScreen::shakeScreen(float power) {
	constexpr u32 nShakes = 5;
	constexpr float iTimeShakes = 0.03f;
	TimerManager::addLinearEase(1.f, "", &_gameViewOffset, { 0, 0 }, { 50, 0 });

	const auto& lambda = [&, p = power, n = nShakes](double)mutable->bool {
		Vector2f from = _gameViewOffset;
		Vector2f to = Vector2f::createUnitVector(unitaryRng(RD) * 2 * PIf) * power;
		TimerManager::addLinearEase(
			iTimeShakes, 
			"shake", 
			&_gameViewOffset, 
			from, 
			to
		);
		if (n-- <= 0)
			TimerManager::addLinearEase(
				iTimeShakes, 
				"shakeBack", 
				&_gameViewOffset, 
				_gameViewOffset, 
				{ 0, 0 }
			);

		return n == 0;
	};

	TimerManager::addFunction(iTimeShakes, "shake", lambda);

}

u32 LevelScreen::getIndex() const {
	return _n;
}
