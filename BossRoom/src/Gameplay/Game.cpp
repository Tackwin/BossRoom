#include "Gameplay/Game.hpp"

#include "Screens/RoomSelectorScreen.hpp"
#include "Managers/AssetsManager.hpp"
#include "Managers/InputsManager.hpp"
#include "Managers/TimerManager.hpp"
#include "Screens/LevelScreen.hpp"
#include "Gameplay/Weapon.hpp"
#include "Gameplay/Player.hpp"
#include "Gameplay/Level.hpp"
#include "Gameplay/Boss.hpp"


using namespace nlohmann;

Game::Game()
	: _player(std::make_shared<Player>(AssetsManager::getJson(JSON_KEY)["player"])) {
	
	Weapon::createWeapons(_player);
	Boss::createBosses();
	Level::createLevels();

	auto weapon = std::make_shared<Weapon>(*Weapon::_weapons[0]);
	_player->_weapon = weapon;

	_debugText["ups"].setFont(AssetsManager::getFont("consola"));
	_debugText["ups"].setCharacterSize(15);
	_debugText["ups"].setPosition(5, 5);

	_debugText["#Entity"].setFont(AssetsManager::getFont("consola"));
	_debugText["#Entity"].setCharacterSize(15);
	_debugText["#Entity"].setPosition(5, 20);

	enterScreen(std::make_shared<RoomSelectorScreen>(0));
}

Game::~Game() {
	_screens.top()->onExit();
	Level::destroyLevels();
	Boss::destroyBosses();
}

void Game::enterRoom(uint32 n) {
	enterScreen(std::make_shared<LevelScreen>(n));
}

void Game::enterScreen(std::shared_ptr<Screen> s) {
	if (!_screens.empty()) {
		_screens.top()->onExit();
	}
	s->onEnter();
	_screens.push(s);
}
void Game::exitScreen() {
	if (_screens.empty())
		return;

	_screens.top()->onExit();
	_screens.pop();
	if(!_screens.empty())
		_screens.top()->onEnter();
}



void Game::update(float dt) {
	updateDebugText(dt);
	_screens.top()->update(dt);
	/*
	//On switch le mode, soit au cac, soit a distance
	if (_distance && (_player->_pos - _level->_boss->_pos).length2() < (150 + _player->_radius + _level->_boss->_radius) * (150 + _player->_radius + _level->_boss->_radius)) {
		TimerManager::addSquaredIOEase<Vector2>(0.5f, "cameraTranslate", &_gameViewPos, _gameView.getCenter(), _level->_boss->_pos);
		TimerManager::addSquaredIOEase<Vector2>(0.5f, "cameraZoom", &_gameViewSize, _gameView.getSize(), { C::WIDTH / 1.5f, C::HEIGHT / 1.5f });
		_distance = false;
	}
	else if (!_distance && (_player->_pos - _level->_boss->_pos).length2() > (250 + _player->_radius + _level->_boss->_radius) * (250 + _player->_radius + _level->_boss->_radius)) {
		TimerManager::addSquaredIOEase<Vector2>(0.5f, "cameraTranslate", &_gameViewPos, _level->_boss->_pos, { C::WIDTH / 2.f, C::HEIGHT / 2.f });
		TimerManager::addSquaredIOEase<Vector2>(0.5f, "cameraZoom", &_gameViewSize, _gameView.getSize(), { C::WIDTH / 1.f, C::HEIGHT / 1.f });
		_distance = true;
	}
	*/
}

void Game::updateDebugText(float dt) {
	static float avgMs = 0;
	static int avgN = 0;
	static constexpr int avgRange = MAX_FPS == 0 ? 100 : MAX_FPS;

	avgMs += dt;
	if (++avgN > avgRange) {
		uint32_t fps = (uint32_t)(avgRange / (avgMs == .0f ? 1 : avgMs));
		uint32_t µs = (uint32_t)(1000'000 * avgMs / avgRange); // :)
		_debugText["ups"].setString("Fps: " + std::to_string(fps) + "\tµs: " + std::to_string(µs));
		//_debugText["#Entity"].setString("#Entity: " + std::to_string(ecs.size()));
		
		avgN = 0;
		avgMs = 0;
	}

}

void Game::render(sf::RenderTarget& target) {
	auto& initView = target.getView();

	_screens.top()->render(target);
	for (auto& [_,v] : _debugText) {
		target.draw(v);
	}

	target.setView(initView);
}

void Game::nextRoom() {
	if (auto screen = dynamic_cast<LevelScreen*>(_screens.top().get())) {
		enterRoom(screen->_n + 1);
	}
}
