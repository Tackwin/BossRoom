#include "Game.hpp"

#include "Common.hpp"

#include "Managers/AssetsManager.hpp"
#include "Managers/InputsManager.hpp"
#include "Managers/MemoryManager.hpp"
#include "Managers/TimerManager.hpp"

#include "Screens/RoomSelectorScreen.hpp"
#include "Screens/LevelScreen.hpp"
#include "Screens/StartScreen.hpp"

#include "Gameplay/Weapon.hpp"
#include "Gameplay/Player.hpp"
#include "Gameplay/Level.hpp"
#include "Gameplay/Boss.hpp"

void Game::start() {
	enterScreen(MM::make_shared<StartScreen>());
}

void Game::enterDungeon() {
	enterRoom(0u);
}

void Game::nextRoom() {
	if (
		auto screen = dynamic_cast<LevelScreen*>(_screens.top().get()); 
		screen
	) {
		enterRoom(screen->getIndex() + 1);
	}
}

void Game::render(sf::RenderTarget& target) {
	auto initialView = target.getView();

	_screens.top()->render(target);
	target.setView(initialView);
	for (auto& [_, v] : _debugText) {
		target.draw(v);
	}

	target.setView(initialView);
}

void Game::updateDebugText(double dt) {
	static double avgMs = 0;
	static int avgN = 0;
	static constexpr int avgRange = MAX_FPS == 0 ? 100 : MAX_FPS;

	avgMs += dt;
	if (++avgN > avgRange) {
		u32 fps = (u32)(avgRange / (avgMs == .0f ? 1 : avgMs));
		u32 µs = (u32)(1000'000 * avgMs / avgRange); // :)
		_debugText["ups"].setString(
			"Fps: " + std::to_string(fps) + "\tµs: " + std::to_string(µs)
		);

		avgN = 0;
		avgMs = 0;
	}
}

void Game::update(double dt) {
	updateDebugText(dt);
	_screens.top()->update(dt);
}

void Game::exitScreen() {
	pop();
	if (!_screens.empty())
		_screens.top()->onEnter();
}

void Game::enterScreen(std::shared_ptr<Screen> s) {
	if (!_screens.empty()) {
		_screens.top()->onExit();
	}
	s->onEnter();
	_screens.push(s);
}

void Game::enterRoom(u32 n) {
	enterScreen(MM::make_shared<LevelScreen>(n));
}

Game::~Game() {
	for (u32 i = _screens.size(); i > 0u; --i) {
		pop();
	}

	Level::destroyLevels();
	Boss::destroyBosses();
}

Game::Game() : 
	_player(
		MM::make_shared<Player>(AssetsManager::getJson(JSON_KEY)["player"])
	)
{
	Weapon::createWeapons(_player);
	Boss::createBosses();
	Level::createLevels();

	_player->swapWeapon(MM::make_shared<Weapon>(Weapon::_weapons[0]));

	_debugText["ups"].setFont(AssetsManager::getFont("consola"));
	_debugText["ups"].setCharacterSize(20);
	_debugText["ups"].setPosition(5, 5);
	_debugText["ups"].setFillColor(sf::Color(100, 100, 100));
}

void Game::pop() {
	if (_screens.empty()) return;

	_screens.top()->onExit();
	_screens.pop();
}

Player* Game::getPlayer() const {
	return _player.get();
}
