#include "Gameplay/Game.hpp"

#include "Screens/RoomSelectorScreen.hpp"
#include "Managers/AssetsManager.hpp"
#include "Managers/InputsManager.hpp"
#include "Managers/TimerManager.hpp"
#include "Screens/LevelScreen.hpp"
#include "Screens/StartScreen.hpp"
#include "Gameplay/Weapon.hpp"
#include "Gameplay/Player.hpp"
#include "Gameplay/Level.hpp"
#include "Gameplay/Boss.hpp"


using namespace nlohmann;

Game::Game() :
	_player(std::make_shared<Player>(AssetsManager::getJson(JSON_KEY)["player"])),
	_debugTimeClockShape(50.f)
{
	Weapon::createWeapons(_player);
	Boss::createBosses();
	Level::createLevels();

	auto weapon = std::make_shared<Weapon>(*Weapon::_weapons[0]);
	_player->swapWeapon(weapon);

	_debugText["ups"].setFont(AssetsManager::getFont("consola"));
	_debugText["ups"].setCharacterSize(20);
	_debugText["ups"].setPosition(5, 5);
	_debugText["ups"].setFillColor(sf::Color(100, 100, 100));

	TimerManager::addFunction(1.f / 255.f, "debugTimeClock", [&](auto)mutable->bool {
		_debugTimeClockColor++;
		return false;
	});
	_debugTimeClockShape.setOrigin(50.f, 50.f);
	_debugTimeClockShape.setPosition(50.f, 50.f);
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

	_debugTimeClockShape.setFillColor(sf::Color(_debugTimeClockColor, _debugTimeClockColor, _debugTimeClockColor));
	target.draw(_debugTimeClockShape);
}

void Game::nextRoom() {
	if (auto screen = dynamic_cast<LevelScreen*>(_screens.top().get())) {
		enterRoom(screen->_n + 1);
	}
}

void Game::enterDungeon() {
	enterScreen(std::make_shared<RoomSelectorScreen>(0));
}

void Game::start() {
	enterScreen(std::make_shared<StartScreen>());
}